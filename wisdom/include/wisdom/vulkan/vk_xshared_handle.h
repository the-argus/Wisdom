#pragma once
#ifndef WISDOM_MODULES
#include <vulkan/vulkan.hpp>
#include <atomic>
#endif

#if VK_HEADER_VERSION < 257
namespace VULKAN_HPP_NAMESPACE {
template<typename HandleType>
class SharedHandleTraits;
class NoParent;

template<class HandleType>
using default_vk_deleter = typename vk::UniqueHandleTraits<HandleType, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>::deleter;

template<class HandleType>
using parent = decltype(std::declval<default_vk_deleter<HandleType>>().getOwner());

template<typename HandleType, typename = void>
struct HasParentType : std::false_type {
};

template<typename HandleType>
struct HasParentType<HandleType, decltype((void)parent<HandleType>())> : std::true_type {
};

template<typename HandleType, typename Enable = void>
struct GetParentType {
    using type = NoParent;
};

template<typename HandleType>
struct GetParentType<HandleType, typename std::enable_if<HasParentType<HandleType>::value>::type> {
    using type = parent<HandleType>;
};

template<class HandleType>
using parent_of_t = typename GetParentType<HandleType>::type;

template<class HandleType>
constexpr inline bool has_parent = !std::is_same<parent_of_t<HandleType>, NoParent>::value;

//=====================================================================================================================

template<typename HandleType>
class SharedHandle;

template<typename ParentType, typename Deleter>
struct SharedHeader {
    SharedHeader(SharedHandle<ParentType> parent, Deleter deleter = Deleter()) VULKAN_HPP_NOEXCEPT
        : parent(std::move(parent)),
          deleter(std::move(deleter))
    {
    }

    SharedHandle<ParentType> parent{};
    Deleter deleter;
};

template<typename Deleter>
struct SharedHeader<NoParent, Deleter> {
    SharedHeader(Deleter deleter = Deleter()) VULKAN_HPP_NOEXCEPT : deleter(std::move(deleter)) { }

    Deleter deleter;
};

//=====================================================================================================================

template<typename HeaderType>
class ReferenceCounter
{
public:
    template<typename... Args>
    ReferenceCounter(Args&&... control_args)
        : m_header(std::forward<Args>(control_args)...){};
    ReferenceCounter(const ReferenceCounter&) = delete;
    ReferenceCounter& operator=(const ReferenceCounter&) = delete;

    size_t addRef() VULKAN_HPP_NOEXCEPT
    {
        return m_ref_cnt.fetch_add(1, std::memory_order_relaxed);
    }

    size_t release() VULKAN_HPP_NOEXCEPT
    {
        return m_ref_cnt.fetch_sub(1, std::memory_order_acq_rel);
    }

    std::atomic_size_t m_ref_cnt{ 1 };
    HeaderType m_header{};
};

//=====================================================================================================================

template<typename HandleType, typename HeaderType, typename ForwardType = SharedHandle<HandleType>>
class SharedHandleBase
{
public:
    using ParentType = parent_of_t<HandleType>;

    SharedHandleBase() = default;

    template<typename... Args>
    SharedHandleBase(HandleType handle, Args&&... control_args)
        : m_control(new ReferenceCounter<HeaderType>(std::forward<Args>(control_args)...)), m_handle(handle)
    {
    }

    SharedHandleBase(const SharedHandleBase& o) VULKAN_HPP_NOEXCEPT
    {
        o.addRef();
        m_handle = o.m_handle;
        m_control = o.m_control;
    }

    SharedHandleBase(SharedHandleBase&& o) VULKAN_HPP_NOEXCEPT
        : m_handle(o.m_handle),
          m_control(o.m_control)
    {
        o.m_handle = nullptr;
        o.m_control = nullptr;
    }

    SharedHandleBase& operator=(const SharedHandleBase& o) VULKAN_HPP_NOEXCEPT
    {
        SharedHandleBase(o).swap(*this);
        return *this;
    }

    SharedHandleBase& operator=(SharedHandleBase&& o) VULKAN_HPP_NOEXCEPT
    {
        SharedHandleBase(std::move(o)).swap(*this);
        return *this;
    }

    ~SharedHandleBase()
    {
        // only this function owns the last reference to the control block
        // the same principle is used in the default deleter of std::shared_ptr
        if (m_control && (m_control->release() == 0)) {
            ForwardType::internalDestroy(getHeader(), m_handle);
            delete m_control;
        }
    }

    [[nodiscard]] HandleType get() const VULKAN_HPP_NOEXCEPT
    {
        return m_handle;
    }

    explicit operator bool() const VULKAN_HPP_NOEXCEPT
    {
        return bool(m_handle);
    }

    const HandleType* operator->() const VULKAN_HPP_NOEXCEPT
    {
        return &m_handle;
    }

    HandleType* operator->() VULKAN_HPP_NOEXCEPT
    {
        return &m_handle;
    }

    void reset() VULKAN_HPP_NOEXCEPT
    {
        SharedHandleBase().swap(*this);
    }

    void swap(SharedHandleBase& o) VULKAN_HPP_NOEXCEPT
    {
        std::swap(m_handle, o.m_handle);
        std::swap(m_control, o.m_control);
    }

    template<typename T = HandleType>
    typename std::enable_if<has_parent<T>, const SharedHandle<ParentType>&>::type getParent() const VULKAN_HPP_NOEXCEPT
    {
        return getHeader().parent;
    }

protected:
    template<typename T = HandleType>
    static typename std::enable_if<!has_parent<T>, void>::type internalDestroy(const HeaderType& control, HandleType handle) VULKAN_HPP_NOEXCEPT
    {
        control.deleter.destroy(handle);
    }

    template<typename T = HandleType>
    static typename std::enable_if<has_parent<T>, void>::type internalDestroy(const HeaderType& control, HandleType handle) VULKAN_HPP_NOEXCEPT
    {
        control.deleter.destroy(control.parent.get(), handle);
    }

    [[nodiscard]] const HeaderType& getHeader() const VULKAN_HPP_NOEXCEPT
    {
        return m_control->m_header;
    }

private:
    void addRef() const VULKAN_HPP_NOEXCEPT
    {
        if (m_control)
            m_control->addRef();
    }

protected:
    ReferenceCounter<HeaderType>* m_control = nullptr;
    HandleType m_handle{};
};

template<typename HandleType>
class SharedHandle : public SharedHandleBase<HandleType, SharedHeader<parent_of_t<HandleType>, typename SharedHandleTraits<HandleType>::deleter>>
{
private:
    using BaseType = SharedHandleBase<HandleType, SharedHeader<parent_of_t<HandleType>, typename SharedHandleTraits<HandleType>::deleter>>;
    using DeleterType = typename SharedHandleTraits<HandleType>::deleter;
    friend BaseType;

public:
    using element_type = HandleType;

    SharedHandle() = default;

    template<typename T = HandleType, typename = typename std::enable_if<has_parent<T>>::type>
    explicit SharedHandle(HandleType handle, SharedHandle<typename BaseType::ParentType> parent, DeleterType deleter = DeleterType()) VULKAN_HPP_NOEXCEPT
        : BaseType(handle, std::move(parent), std::move(deleter))
    {
    }

    template<typename T = HandleType, typename = typename std::enable_if<!has_parent<T>>::type>
    explicit SharedHandle(HandleType handle, DeleterType deleter = DeleterType()) VULKAN_HPP_NOEXCEPT : BaseType(handle, std::move(deleter))
    {
    }

protected:
    using BaseType::internalDestroy;
};

template<typename SharedType>
VULKAN_HPP_INLINE std::vector<typename SharedType::element_type> sharedToRaw(std::vector<SharedType> const& handles)
{
    std::vector<typename SharedType::element_type> newBuffer(handles.size());
    std::transform(handles.begin(), handles.end(), newBuffer.begin(), [](SharedType const& handle) { return handle.get(); });
    return newBuffer;
}

template<typename HandleType>
class ObjectDestroyShared
{
public:
    using ParentType = parent_of_t<HandleType>;

    template<class Dispatcher>
    using destroy_pfn_t = typename std::conditional<has_parent<HandleType>,

                                                    void (ParentType::*)(HandleType, const AllocationCallbacks*, const Dispatcher&) const,

                                                    void (HandleType::*)(const AllocationCallbacks*, const Dispatcher&) const>::type;
    using selector_t = typename std::conditional<has_parent<HandleType>, ParentType, HandleType>::type;

    template<typename Dispatcher = VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>
    ObjectDestroyShared(Optional<const AllocationCallbacks> allocationCallbacks VULKAN_HPP_DEFAULT_ARGUMENT_NULLPTR_ASSIGNMENT,
                        const Dispatcher& dispatch VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT)
        : m_destroy(reinterpret_cast<decltype(m_destroy)>(static_cast<destroy_pfn_t<Dispatcher>>(&selector_t::destroy)))
        , m_dispatch(&dispatch)
        , m_allocationCallbacks(allocationCallbacks)
    {
    }

    template<typename T = HandleType>
    typename std::enable_if<has_parent<T>, void>::type destroy(ParentType parent, HandleType handle) const VULKAN_HPP_NOEXCEPT
    {
        VULKAN_HPP_ASSERT(m_destroy && m_dispatch);
        (parent.*m_destroy)(handle, m_allocationCallbacks, *m_dispatch);
    }

    template<typename T = HandleType>
    typename std::enable_if<!has_parent<T>, void>::type destroy(HandleType handle) const VULKAN_HPP_NOEXCEPT
    {
        VULKAN_HPP_ASSERT(m_destroy && m_dispatch);
        (handle.*m_destroy)(m_allocationCallbacks, *m_dispatch);
    }

private:
    destroy_pfn_t<DispatchLoaderBase> m_destroy = nullptr;
    const DispatchLoaderBase* m_dispatch = nullptr;
    Optional<const AllocationCallbacks> m_allocationCallbacks = nullptr;
};

//================================================================================================

template<typename HandleType>
class ObjectFreeShared
{
public:
    using ParentType = parent_of_t<HandleType>;

    template<class Dispatcher>
    using destroy_pfn_t = void (ParentType::*)(HandleType, const AllocationCallbacks*, const Dispatcher&) const;

    template<class Dispatcher = VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>
    ObjectFreeShared(Optional<const AllocationCallbacks> allocationCallbacks VULKAN_HPP_DEFAULT_ARGUMENT_NULLPTR_ASSIGNMENT,
                     const Dispatcher& dispatch VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT)
        : m_destroy(reinterpret_cast<decltype(m_destroy)>(static_cast<destroy_pfn_t<Dispatcher>>(&ParentType::free)))
        , m_dispatch(&dispatch)
        , m_allocationCallbacks(allocationCallbacks)
    {
    }

    void destroy(ParentType parent, HandleType handle) const VULKAN_HPP_NOEXCEPT
    {
        VULKAN_HPP_ASSERT(m_destroy && m_dispatch);
        (parent.*m_destroy)(handle, m_allocationCallbacks, *m_dispatch);
    }

private:
    destroy_pfn_t<DispatchLoaderBase> m_destroy = nullptr;
    const DispatchLoaderBase* m_dispatch = nullptr;
    Optional<const AllocationCallbacks> m_allocationCallbacks = nullptr;
};

//================================================================================================

template<typename HandleType>
class ObjectReleaseShared
{
public:
    using ParentType = parent_of_t<HandleType>;

    template<class Dispatcher>
    using destroy_pfn_t = void (ParentType::*)(HandleType, const Dispatcher&) const;

    template<class Dispatcher = VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>
    ObjectReleaseShared(Optional<const AllocationCallbacks> /*allocationCallbacks*/ VULKAN_HPP_DEFAULT_ARGUMENT_NULLPTR_ASSIGNMENT,
                        const Dispatcher& dispatch VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT)
        : m_destroy(reinterpret_cast<decltype(m_destroy)>(static_cast<destroy_pfn_t<Dispatcher>>(&ParentType::release))), m_dispatch(&dispatch)
    {
    }

    void destroy(ParentType parent, HandleType handle) const VULKAN_HPP_NOEXCEPT
    {
        VULKAN_HPP_ASSERT(m_destroy && m_dispatch);
        (parent.*m_destroy)(handle, *m_dispatch);
    }

private:
    destroy_pfn_t<DispatchLoaderBase> m_destroy = nullptr;
    const DispatchLoaderBase* m_dispatch = nullptr;
};

//================================================================================================

template<typename HandleType, typename PoolType>
class PoolFreeShared
{
public:
    using ParentType = parent_of_t<HandleType>;

    template<class Dispatcher>
    using ret_t = decltype(std::declval<ParentType>().free(PoolType(), 0u, nullptr, Dispatcher()));

    template<class Dispatcher>
    using destroy_pfn_t = ret_t<Dispatcher> (ParentType::*)(PoolType, uint32_t, const HandleType*, const Dispatcher&) const;

    PoolFreeShared() = default;

    template<class Dispatcher = VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>
    PoolFreeShared(SharedHandle<PoolType> pool, const Dispatcher& dispatch VULKAN_HPP_DEFAULT_DISPATCHER_ASSIGNMENT)
        : m_destroy(reinterpret_cast<decltype(m_destroy)>(static_cast<destroy_pfn_t<Dispatcher>>(&ParentType::free)))
        , m_pool(std::move(pool))
        , m_dispatch(&dispatch)
    {
    }

    void destroy(ParentType parent, HandleType handle) const VULKAN_HPP_NOEXCEPT
    {
        VULKAN_HPP_ASSERT(m_destroy && m_dispatch);
        (parent.*m_destroy)(m_pool.get(), 1u, &handle, *m_dispatch);
    }

private:
    destroy_pfn_t<DispatchLoaderBase> m_destroy = nullptr;
    const DispatchLoaderBase* m_dispatch = nullptr;
    SharedHandle<PoolType> m_pool{};
};

template<>
class SharedHandleTraits<Instance>
{
public:
    using deleter = ObjectDestroyShared<Instance>;
};

using SharedInstance = SharedHandle<Instance>;

template<>
class SharedHandleTraits<Device>
{
public:
    using deleter = ObjectDestroyShared<Device>;
};

using SharedDevice = SharedHandle<Device>;

template<>
class SharedHandleTraits<DeviceMemory>
{
public:
    using deleter = ObjectFreeShared<DeviceMemory>;
};

using SharedDeviceMemory = SharedHandle<DeviceMemory>;

template<>
class SharedHandleTraits<Fence>
{
public:
    using deleter = ObjectDestroyShared<Fence>;
};

using SharedFence = SharedHandle<Fence>;

template<>
class SharedHandleTraits<Semaphore>
{
public:
    using deleter = ObjectDestroyShared<Semaphore>;
};

using SharedSemaphore = SharedHandle<Semaphore>;

template<>
class SharedHandleTraits<Event>
{
public:
    using deleter = ObjectDestroyShared<Event>;
};

using SharedEvent = SharedHandle<Event>;

template<>
class SharedHandleTraits<QueryPool>
{
public:
    using deleter = ObjectDestroyShared<QueryPool>;
};

using SharedQueryPool = SharedHandle<QueryPool>;

template<>
class SharedHandleTraits<Buffer>
{
public:
    using deleter = ObjectDestroyShared<Buffer>;
};

using SharedBuffer = SharedHandle<Buffer>;

template<>
class SharedHandleTraits<BufferView>
{
public:
    using deleter = ObjectDestroyShared<BufferView>;
};

using SharedBufferView = SharedHandle<BufferView>;

template<>
class SharedHandleTraits<Image>
{
public:
    using deleter = ObjectDestroyShared<Image>;
};

using SharedImage = SharedHandle<Image>;

template<>
class SharedHandleTraits<ImageView>
{
public:
    using deleter = ObjectDestroyShared<ImageView>;
};

using SharedImageView = SharedHandle<ImageView>;

template<>
class SharedHandleTraits<ShaderModule>
{
public:
    using deleter = ObjectDestroyShared<ShaderModule>;
};

using SharedShaderModule = SharedHandle<ShaderModule>;

template<>
class SharedHandleTraits<PipelineCache>
{
public:
    using deleter = ObjectDestroyShared<PipelineCache>;
};

using SharedPipelineCache = SharedHandle<PipelineCache>;

template<>
class SharedHandleTraits<Pipeline>
{
public:
    using deleter = ObjectDestroyShared<Pipeline>;
};

using SharedPipeline = SharedHandle<Pipeline>;

template<>
class SharedHandleTraits<PipelineLayout>
{
public:
    using deleter = ObjectDestroyShared<PipelineLayout>;
};

using SharedPipelineLayout = SharedHandle<PipelineLayout>;

template<>
class SharedHandleTraits<Sampler>
{
public:
    using deleter = ObjectDestroyShared<Sampler>;
};

using SharedSampler = SharedHandle<Sampler>;

template<>
class SharedHandleTraits<DescriptorPool>
{
public:
    using deleter = ObjectDestroyShared<DescriptorPool>;
};

using SharedDescriptorPool = SharedHandle<DescriptorPool>;

template<>
class SharedHandleTraits<DescriptorSet>
{
public:
    using deleter = PoolFreeShared<DescriptorSet, DescriptorPool>;
};

using SharedDescriptorSet = SharedHandle<DescriptorSet>;

template<>
class SharedHandleTraits<DescriptorSetLayout>
{
public:
    using deleter = ObjectDestroyShared<DescriptorSetLayout>;
};

using SharedDescriptorSetLayout = SharedHandle<DescriptorSetLayout>;

template<>
class SharedHandleTraits<Framebuffer>
{
public:
    using deleter = ObjectDestroyShared<Framebuffer>;
};

using SharedFramebuffer = SharedHandle<Framebuffer>;

template<>
class SharedHandleTraits<RenderPass>
{
public:
    using deleter = ObjectDestroyShared<RenderPass>;
};

using SharedRenderPass = SharedHandle<RenderPass>;

template<>
class SharedHandleTraits<CommandPool>
{
public:
    using deleter = ObjectDestroyShared<CommandPool>;
};

using SharedCommandPool = SharedHandle<CommandPool>;

template<>
class SharedHandleTraits<CommandBuffer>
{
public:
    using deleter = PoolFreeShared<CommandBuffer, CommandPool>;
};

using SharedCommandBuffer = SharedHandle<CommandBuffer>;

//=== VK_VERSION_1_1 ===
template<>
class SharedHandleTraits<SamplerYcbcrConversion>
{
public:
    using deleter = ObjectDestroyShared<SamplerYcbcrConversion>;
};

using SharedSamplerYcbcrConversion = SharedHandle<SamplerYcbcrConversion>;
using SharedSamplerYcbcrConversionKHR = SharedHandle<SamplerYcbcrConversion>;

template<>
class SharedHandleTraits<DescriptorUpdateTemplate>
{
public:
    using deleter = ObjectDestroyShared<DescriptorUpdateTemplate>;
};

using SharedDescriptorUpdateTemplate = SharedHandle<DescriptorUpdateTemplate>;
using SharedDescriptorUpdateTemplateKHR = SharedHandle<DescriptorUpdateTemplate>;

//=== VK_VERSION_1_3 ===
template<>
class SharedHandleTraits<PrivateDataSlot>
{
public:
    using deleter = ObjectDestroyShared<PrivateDataSlot>;
};

using SharedPrivateDataSlot = SharedHandle<PrivateDataSlot>;
using SharedPrivateDataSlotEXT = SharedHandle<PrivateDataSlot>;

//=== VK_KHR_surface ===
template<>
class SharedHandleTraits<SurfaceKHR>
{
public:
    using deleter = ObjectDestroyShared<SurfaceKHR>;
};

using SharedSurfaceKHR = SharedHandle<SurfaceKHR>;

//=== VK_KHR_swapchain ===
template<>
class SharedHandleTraits<SwapchainKHR>
{
public:
    using deleter = ObjectDestroyShared<SwapchainKHR>;
};

using SharedSwapchainKHR = SharedHandle<SwapchainKHR>;

//=== VK_EXT_debug_report ===
template<>
class SharedHandleTraits<DebugReportCallbackEXT>
{
public:
    using deleter = ObjectDestroyShared<DebugReportCallbackEXT>;
};

using SharedDebugReportCallbackEXT = SharedHandle<DebugReportCallbackEXT>;

//=== VK_KHR_video_queue ===
template<>
class SharedHandleTraits<VideoSessionKHR>
{
public:
    using deleter = ObjectDestroyShared<VideoSessionKHR>;
};

using SharedVideoSessionKHR = SharedHandle<VideoSessionKHR>;

template<>
class SharedHandleTraits<VideoSessionParametersKHR>
{
public:
    using deleter = ObjectDestroyShared<VideoSessionParametersKHR>;
};

using SharedVideoSessionParametersKHR = SharedHandle<VideoSessionParametersKHR>;

//=== VK_NVX_binary_import ===
template<>
class SharedHandleTraits<CuModuleNVX>
{
public:
    using deleter = ObjectDestroyShared<CuModuleNVX>;
};

using SharedCuModuleNVX = SharedHandle<CuModuleNVX>;

template<>
class SharedHandleTraits<CuFunctionNVX>
{
public:
    using deleter = ObjectDestroyShared<CuFunctionNVX>;
};

using SharedCuFunctionNVX = SharedHandle<CuFunctionNVX>;

//=== VK_EXT_debug_utils ===
template<>
class SharedHandleTraits<DebugUtilsMessengerEXT>
{
public:
    using deleter = ObjectDestroyShared<DebugUtilsMessengerEXT>;
};

using SharedDebugUtilsMessengerEXT = SharedHandle<DebugUtilsMessengerEXT>;

//=== VK_KHR_acceleration_structure ===
template<>
class SharedHandleTraits<AccelerationStructureKHR>
{
public:
    using deleter = ObjectDestroyShared<AccelerationStructureKHR>;
};

using SharedAccelerationStructureKHR = SharedHandle<AccelerationStructureKHR>;

//=== VK_EXT_validation_cache ===
template<>
class SharedHandleTraits<ValidationCacheEXT>
{
public:
    using deleter = ObjectDestroyShared<ValidationCacheEXT>;
};

using SharedValidationCacheEXT = SharedHandle<ValidationCacheEXT>;

//=== VK_NV_ray_tracing ===
template<>
class SharedHandleTraits<AccelerationStructureNV>
{
public:
    using deleter = ObjectDestroyShared<AccelerationStructureNV>;
};

using SharedAccelerationStructureNV = SharedHandle<AccelerationStructureNV>;

//=== VK_KHR_deferred_host_operations ===
template<>
class SharedHandleTraits<DeferredOperationKHR>
{
public:
    using deleter = ObjectDestroyShared<DeferredOperationKHR>;
};

using SharedDeferredOperationKHR = SharedHandle<DeferredOperationKHR>;

//=== VK_NV_device_generated_commands ===
template<>
class SharedHandleTraits<IndirectCommandsLayoutNV>
{
public:
    using deleter = ObjectDestroyShared<IndirectCommandsLayoutNV>;
};

using SharedIndirectCommandsLayoutNV = SharedHandle<IndirectCommandsLayoutNV>;

#if defined(VK_USE_PLATFORM_FUCHSIA)
//=== VK_FUCHSIA_buffer_collection ===
template<>
class SharedHandleTraits<BufferCollectionFUCHSIA>
{
public:
    using deleter = ObjectDestroyShared<BufferCollectionFUCHSIA>;
};

using SharedBufferCollectionFUCHSIA = SharedHandle<BufferCollectionFUCHSIA>;
#endif /*VK_USE_PLATFORM_FUCHSIA*/

//=== VK_EXT_opacity_micromap ===
template<>
class SharedHandleTraits<MicromapEXT>
{
public:
    using deleter = ObjectDestroyShared<MicromapEXT>;
};

using SharedMicromapEXT = SharedHandle<MicromapEXT>;

//=== VK_NV_optical_flow ===

#ifdef VK_NV_optical_flow
template<>
class SharedHandleTraits<OpticalFlowSessionNV>
{
public:
    using deleter = ObjectDestroyShared<OpticalFlowSessionNV>;
};

using SharedOpticalFlowSessionNV = SharedHandle<OpticalFlowSessionNV>;
#endif

//=== VK_EXT_shader_object ===

#ifdef VK_EXT_shader_object
template<>
class SharedHandleTraits<ShaderEXT>
{
public:
    using deleter = ObjectDestroyShared<ShaderEXT>;
};

using SharedShaderEXT = SharedHandle<ShaderEXT>;
#endif

enum class SwapchainOwns {
    no,
    yes,
};

struct ImageHeader : SharedHeader<parent_of_t<VULKAN_HPP_NAMESPACE::Image>, typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::Image>::deleter> {
    ImageHeader(
            SharedHandle<parent_of_t<VULKAN_HPP_NAMESPACE::Image>> parent,
            typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::Image>::deleter deleter = typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::Image>::deleter(),
            SwapchainOwns swapchainOwned = SwapchainOwns::no) VULKAN_HPP_NOEXCEPT
        : SharedHeader<parent_of_t<VULKAN_HPP_NAMESPACE::Image>, typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::Image>::deleter>(std::move(parent),
                                                                                                                                                          deleter),
          swapchainOwned(swapchainOwned)
    {
    }

    SwapchainOwns swapchainOwned = SwapchainOwns::no;
};

template<>
class SharedHandle<VULKAN_HPP_NAMESPACE::Image> : public SharedHandleBase<VULKAN_HPP_NAMESPACE::Image, ImageHeader>
{
    using BaseType = SharedHandleBase<VULKAN_HPP_NAMESPACE::Image, ImageHeader>;
    using DeleterType = typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::Image>::deleter;
    friend BaseType;

public:
    using element_type = VULKAN_HPP_NAMESPACE::Image;

    SharedHandle() = default;

    explicit SharedHandle(VULKAN_HPP_NAMESPACE::Image handle,
                          SharedHandle<typename BaseType::ParentType> parent,
                          SwapchainOwns swapchain_owned = SwapchainOwns::no,
                          DeleterType deleter = DeleterType()) VULKAN_HPP_NOEXCEPT
        : BaseType(handle, std::move(parent), deleter, swapchain_owned)
    {
    }

protected:
    static void internalDestroy(const ImageHeader& control, VULKAN_HPP_NAMESPACE::Image handle) VULKAN_HPP_NOEXCEPT
    {
        if (control.swapchainOwned == SwapchainOwns::no) {
            control.deleter.destroy(control.parent.get(), handle);
        }
    }
};

struct SwapchainHeader {
    SwapchainHeader(SharedHandle<VULKAN_HPP_NAMESPACE::SurfaceKHR> surface,
                    SharedHandle<parent_of_t<VULKAN_HPP_NAMESPACE::SwapchainKHR>> parent,
                    typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::SwapchainKHR>::deleter deleter =
                            typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::SwapchainKHR>::deleter()) VULKAN_HPP_NOEXCEPT
        : surface(std::move(surface)),
          parent(std::move(parent)),
          deleter(deleter)
    {
    }

    SharedHandle<VULKAN_HPP_NAMESPACE::SurfaceKHR> surface{};
    SharedHandle<parent_of_t<VULKAN_HPP_NAMESPACE::SwapchainKHR>> parent{};
    typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::SwapchainKHR>::deleter deleter{};
};

template<>
class SharedHandle<VULKAN_HPP_NAMESPACE::SwapchainKHR> : public SharedHandleBase<VULKAN_HPP_NAMESPACE::SwapchainKHR, SwapchainHeader>
{
    using BaseType = SharedHandleBase<VULKAN_HPP_NAMESPACE::SwapchainKHR, SwapchainHeader>;
    using DeleterType = typename VULKAN_HPP_NAMESPACE::SharedHandleTraits<VULKAN_HPP_NAMESPACE::SwapchainKHR>::deleter;
    friend BaseType;

public:
    using element_type = VULKAN_HPP_NAMESPACE::SwapchainKHR;

    SharedHandle() = default;

    explicit SharedHandle(VULKAN_HPP_NAMESPACE::SwapchainKHR handle,
                          SharedHandle<typename BaseType::ParentType> parent,
                          SharedHandle<VULKAN_HPP_NAMESPACE::SurfaceKHR> surface,
                          DeleterType deleter = DeleterType()) VULKAN_HPP_NOEXCEPT
        : BaseType(handle, std::move(surface), std::move(parent), deleter)
    {
    }

    [[nodiscard]] const SharedHandle<VULKAN_HPP_NAMESPACE::SurfaceKHR>& getSurface() const VULKAN_HPP_NOEXCEPT
    {
        return getHeader().surface;
    }

protected:
    using BaseType::internalDestroy;
};

} // namespace VULKAN_HPP_NAMESPACE
#endif
