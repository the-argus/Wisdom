#pragma once
#ifndef WISDOM_MODULES
#include <wisdom/api/api_barrier.h>
#include <wisdom/vulkan/vk_views.h>
#include <wisdom/vulkan/vk_allocator_handles.h>
#include <wisdom/vulkan/vk_resource.h>
#include <wisdom/global/assertions.h>
#endif // !WISDOM_MODULES

WIS_EXPORT namespace wis
{
    class VKResourceAllocator;

    template<>
    class Internal<VKResourceAllocator>
    {
    public:
        [[nodiscard]] VmaAllocator GetAllocator() const noexcept
        {
            return allocator.get();
        }

    protected:
        wis::shared_handle<vma::Allocator> allocator;
    };

    /// @brief Resource allocator for Vulkan
    class VKResourceAllocator : public QueryInternal<VKResourceAllocator>
    {
    public:
        VKResourceAllocator() = default;
        WIS_INLINE VKResourceAllocator(VKDeviceView device, VKAdapterView adapter);

    public:
        /// @brief Create a buffer that is persistently mapped to the GPU
        /// @param size Size of the buffer
        /// @param flags Type of buffer
        /// @return Buffer object
        [[nodiscard]] VKBuffer CreatePersistentBuffer(size_t size, BufferFlags flags = BufferFlags::None) const
        {
            vk::BufferCreateInfo desc{
                {}, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits(flags), vk::SharingMode::eExclusive, 0, nullptr, nullptr
            };
            vma::AllocationCreateInfo alloc{
                {}, vma::MemoryUsage::eAuto
            };
            return CreateBuffer(desc, alloc);
        }

        /// @brief Create a buffer that is accessible by the CPU and serves as a staging buffer for GPU uploads
        /// @param size Size of the buffer
        /// @return Buffer object
        [[nodiscard]] VKBuffer CreateUploadBuffer(size_t size) const
        {
            vk::BufferCreateInfo desc{
                vk::BufferCreateFlags{}, size, vk::BufferUsageFlagBits::eTransferSrc,
                vk::SharingMode::eExclusive, 0, nullptr, nullptr
            };
            vma::AllocationCreateInfo alloc{
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent // ensure mapping does not need to be flushed
            };

            return CreateBuffer(desc, alloc);
        }

        /// @brief Create a buffer that is accessible by the CPU and GPU
        /// @param size Size of the buffer
        /// @param flags Type of buffer
        /// @return Buffer object
        [[nodiscard]] VKBuffer CreateHostVisibleBuffer(size_t size, BufferFlags flags = BufferFlags::None) const
        {
            vk::BufferCreateInfo desc{
                vk::BufferCreateFlags{}, size, vk::BufferUsageFlagBits(flags),
                vk::SharingMode::eExclusive, 0, nullptr, nullptr
            };
            vma::AllocationCreateInfo alloc{
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent // ensure mapping does not need to be flushed
            };

            return CreateBuffer(desc, alloc);
        }
        
        /// @brief Create a constant buffer that is accessible by the CPU and GPU
        /// This function is equivalent to CreateHostVisibleBuffer, but ensures that the buffer size is 256 byte aligned in debug mode
        /// @param size Size of the buffer
        /// @return Buffer object
        [[nodiscard]] VKBuffer CreateConstantBuffer(size_t size) const
        {
            wis::assert_debug(size % 256 == 0, wis::format("{} is nor 256 byte aligned", size));
            return CreateHostVisibleBuffer(size, BufferFlags::ConstantBuffer);
        }


    private:
        [[nodiscard]] VKBuffer CreateBuffer(const vk::BufferCreateInfo &desc, const vma::AllocationCreateInfo &alloc_desc) const
        {
            auto [a, b] = allocator->createBuffer(desc, alloc_desc);
            return VKBuffer{ wis::shared_handle<vk::Buffer>{ a, allocator.getParent() }, wis::shared_handle<vma::Allocation>{ b, allocator }, desc.size };
        }
    };
}

#if defined(WISDOM_HEADER_ONLY)
#include "impl/vk_allocator.inl"
#endif