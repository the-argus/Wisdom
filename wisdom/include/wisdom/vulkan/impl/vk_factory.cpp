#ifndef VK_FACTORY_CPP
#define VK_FACTORY_CPP
#include <wisdom/vulkan/vk_factory.h>

#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <vector>
#include <array>
#include <wisdom/global/definitions.h>
#include <wisdom/util/misc.h>

namespace wis::detail {
inline constexpr uint32_t order_performance(VkPhysicalDeviceType t)
{
    switch (t) {
    default:
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return 3;
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return 4;
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return 2;
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
        return 1;
    }
}
inline constexpr uint32_t order_power(VkPhysicalDeviceType t)
{
    switch (t) {
    default:
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return 4;
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return 3;
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return 2;
    case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
        return 1;
    }
}

wis::Result VKFactoryGlobals::InitializeFactoryGlobals() noexcept
{
    if (initialized)
        return {};

    wis::Result vr = {};

    std::call_once(
            global_flag, [this, &vr]() {
                vr = InitializeGlobalTable();
                if (vr.status != wis::Status::Ok)
                    return;

                vr = InitializeInstanceExtensions();
                if (vr.status != wis::Status::Ok)
                    return;

                vr = InitializeInstanceLayers();
                if (vr.status != wis::Status::Ok)
                    return;
            });

    initialized = true;
    return vr;
}

wis::Result VKFactoryGlobals::InitializeGlobalTable() noexcept
{
    if (!global_table.Init(lib_token))
        return wis::make_result<FUNC, "Failed to initialize global table">(VK_ERROR_INITIALIZATION_FAILED);
    return {};
}

wis::Result VKFactoryGlobals::InitializeInstanceExtensions() noexcept
{
    auto& gt = wis::detail::VKFactoryGlobals::Instance().global_table;
    uint32_t count = 0;
    gt.vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    auto extensions = wis::detail::make_fixed_allocation<VkExtensionProperties>(count);
    if (!extensions)
        return wis::make_result<FUNC, "Not enough memory">(VK_ERROR_OUT_OF_HOST_MEMORY);

    auto vr = gt.vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.get());
    if (!wis::succeeded(vr))
        return wis::make_result<FUNC, "Failed to enumerate extensions">(vr);

    // may throw
    instance_extensions.reserve(count);
    for (uint32_t i = 0; i < count; i++) {
        instance_extensions.insert(extensions[i].extensionName);
    }
    return {};
}
wis::Result VKFactoryGlobals::InitializeInstanceLayers() noexcept
{
    auto& gt = wis::detail::VKFactoryGlobals::Instance().global_table;
    uint32_t count = 0;
    gt.vkEnumerateInstanceLayerProperties(&count, nullptr);
    auto layers = wis::detail::make_fixed_allocation<VkLayerProperties>(count);
    if (!layers)
        return wis::make_result<FUNC, "Not enough memory">(VK_ERROR_OUT_OF_HOST_MEMORY);

    auto vr = gt.vkEnumerateInstanceLayerProperties(&count, layers.get());
    if (!wis::succeeded(vr))
        return wis::make_result<FUNC, "Failed to enumerate layers">(vr);

    // may throw
    instance_layers.reserve(count);
    for (uint32_t i = 0; i < count; i++) {
        instance_layers.insert(layers[i].layerName);
    }
    return {};
}
} // namespace wis::detail

wis::VKFactory::VKFactory(
        wis::SharedInstance instance, uint32_t api_ver, bool debug) noexcept
    : QueryInternal(std::move(instance), api_ver, debug)
{
}

wis::ResultValue<wis::VKAdapter>
wis::VKFactory::GetAdapter(uint32_t index, AdapterPreference preference) const noexcept
{
    if (index >= adapters.size()) {
        return wis::make_result<FUNC, "Index out of range">(VK_ERROR_UNKNOWN);
    }
    auto& adapter = adapters[index];
    switch (preference) {
    default:
        return adapter.adapter;
    case AdapterPreference::MinConsumption:
        return adapters[adapter.index_consumption].adapter;
    case AdapterPreference::Performance:
        return adapters[adapter.index_performance].adapter;
    }
}

VkResult wis::VKFactory::VKEnumeratePhysicalDevices() noexcept
{
    auto& itable = factory.table();

    std::vector<VkPhysicalDevice> phys_adapters;
    uint32_t count = 0;
    auto vr = factory.table().vkEnumeratePhysicalDevices(factory.get(), &count, nullptr);
    do
        phys_adapters.resize(count);
    while ((vr = itable.vkEnumeratePhysicalDevices(factory.get(), &count,
                                                   phys_adapters.data())) == VK_INCOMPLETE);
    if (!wis::succeeded(vr))
        return vr;

    adapters.resize(count);

    if (phys_adapters.size() > 1) {
        auto indices = std::views::iota(0u, count);
        std::vector<uint32_t> indices_cons(indices.begin(), indices.end());
        std::vector<uint32_t> indices_perf(indices.begin(), indices.end());

        auto less_consumption = [this](VkPhysicalDevice a, VkPhysicalDevice b) {
            auto& itable = factory.table();
            VkPhysicalDeviceProperties a_properties{};
            VkPhysicalDeviceProperties b_properties{};
            itable.vkGetPhysicalDeviceProperties(a, &a_properties);
            itable.vkGetPhysicalDeviceProperties(b, &b_properties);

            return wis::detail::order_power(a_properties.deviceType) > wis::detail::order_power(b_properties.deviceType)
                    ? true
                    : a_properties.limits.maxMemoryAllocationCount >
                            b_properties.limits.maxMemoryAllocationCount;
        };
        auto less_performance = [this](VkPhysicalDevice a, VkPhysicalDevice b) {
            auto& itable = factory.table();
            VkPhysicalDeviceProperties a_properties{};
            VkPhysicalDeviceProperties b_properties{};
            itable.vkGetPhysicalDeviceProperties(a, &a_properties);
            itable.vkGetPhysicalDeviceProperties(b, &b_properties);

            return wis::detail::order_performance(a_properties.deviceType) > wis::detail::order_performance(b_properties.deviceType)
                    ? true
                    : a_properties.limits.maxMemoryAllocationCount >
                            b_properties.limits.maxMemoryAllocationCount;
        };

        std::ranges::sort(indices_cons,
                          [this, &phys_adapters, less_consumption](uint32_t a, uint32_t b) {
                              return less_consumption(phys_adapters[a], phys_adapters[b]);
                          });
        std::ranges::sort(indices_perf,
                          [this, &phys_adapters, less_consumption](uint32_t a, uint32_t b) {
                              return less_consumption(phys_adapters[a], phys_adapters[b]);
                          });

        for (size_t i = 0; i < count; i++) {
            auto& adapter = adapters[i];
            adapter.adapter = VKAdapter{
                factory, phys_adapters[i]
            };
            adapter.index_performance = indices_perf[i];
            adapter.index_consumption = indices_cons[i];
        }
    } else {
        for (size_t i = 0; i < count; i++) {
            auto& adapter = adapters[i];
            adapter.adapter = VKAdapter{
                factory, phys_adapters[i]
            };
            adapter.index_performance = i;
            adapter.index_consumption = i;
        }
    }
    return VK_SUCCESS;
}

#endif // VK_CREATE_FACTORY_CPP
