// GENERATED
#pragma once
#include "vulkan/vk_factory.h"
#include "vulkan/vk_swapchain.h"
#include "vulkan/vk_device.h"

namespace wis {

/**
 * @brief Creates the wis::VKFactory with extensions, specified in extension array.
 * @param debug_layer Enable the debug layer for underlying API.
 * @param extensions The extensions to enable.
 * The extensions are initialized through this array.
 * @param extension_count The number of extensions to enable.
 * @return wis::VKFactory on success (wis::Status::Ok).
 * */
inline wis::ResultValue<wis::VKFactory> VKCreateFactory(bool debug_layer = false, wis::VKFactoryExtension** extensions = nullptr, uint32_t extension_count = 0)
{
    return wis::ImplVKCreateFactory(debug_layer, extensions, extension_count);
}
/**
 * @brief Creates the wis::VKDevice with extensions, specified in extension array.
 * @param adapter The adapter to create the logical device on.
 * @param extensions The extensions to enable.
 * The extensions are initialized through this array.
 * @param extension_count The number of extensions to enable.
 * @param force Create logical device even if some core functionality is absent.
 * The presence of core functionality is checked by the query function.
 * @return wis::VKDevice on success (wis::Status::Ok).
 * */
inline wis::ResultValue<wis::VKDevice> VKCreateDevice(wis::VKAdapter adapter, wis::VKDeviceExtension** extensions = nullptr, uint32_t extension_count = 0, bool force = false)
{
    return wis::ImplVKCreateDevice(adapter, extensions, extension_count, force);
}

//-------------------------------------------------------------------------

} // namespace wis
