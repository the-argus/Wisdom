// Generated by wisdom generator on 2023-11-03 13:38:30.5432961 GMT+1
#pragma once
#include <wisdom/vulkan/xvk_views.h>
#include <wisdom/api/api.h>

namespace wis{
struct VKGraphicsShaderStages{
    wis::VKShaderView vertex;
    wis::VKShaderView hull;
    wis::VKShaderView domain;
    wis::VKShaderView geometry;
    wis::VKShaderView pixel;
};

struct VKGraphicsPipelineDesc{
    wis::VKRootSignatureView root_signature;
    wis::InputLayout input_layout;
    wis::VKGraphicsShaderStages shaders;
};

inline constexpr VkShaderStageFlagBits convert_vk(ShaderStages value) noexcept{
    switch(value){
    default: return VK_SHADER_STAGE_ALL;
    case ShaderStages::All: return VK_SHADER_STAGE_ALL;
    case ShaderStages::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderStages::Hull: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case ShaderStages::Domain: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    case ShaderStages::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
    case ShaderStages::Pixel: return VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderStages::Amplification: return VK_SHADER_STAGE_TASK_BIT_NV;
    case ShaderStages::Mesh: return VK_SHADER_STAGE_MESH_BIT_NV;
    }
}
inline constexpr VkFormat convert_vk(DataFormat value) noexcept{
    switch(value){
    default: return {};
    case DataFormat::RGBA32Float: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case DataFormat::RGBA32Uint: return VK_FORMAT_R32G32B32A32_UINT;
    case DataFormat::RGBA32Sint: return VK_FORMAT_R32G32B32A32_SINT;
    case DataFormat::RGB32Float: return VK_FORMAT_R32G32B32_SFLOAT;
    case DataFormat::RGB32Uint: return VK_FORMAT_R32G32B32_UINT;
    case DataFormat::RGB32Sint: return VK_FORMAT_R32G32B32_SINT;
    case DataFormat::RGBA16Float: return VK_FORMAT_R16G16B16A16_SFLOAT;
    case DataFormat::RGBA16Unorm: return VK_FORMAT_R16G16B16A16_UNORM;
    case DataFormat::RGBA16Uint: return VK_FORMAT_R16G16B16A16_UINT;
    case DataFormat::RGBA16Snorm: return VK_FORMAT_R16G16B16A16_SNORM;
    case DataFormat::RGBA16Sint: return VK_FORMAT_R16G16B16A16_SINT;
    case DataFormat::RG32Float: return VK_FORMAT_R32G32_SFLOAT;
    case DataFormat::RG32Uint: return VK_FORMAT_R32G32_UINT;
    case DataFormat::RG32Sint: return VK_FORMAT_R32G32_SINT;
    case DataFormat::D32FloatS8Uint: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case DataFormat::RGB10A2Unorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    case DataFormat::RGB10A2Uint: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
    case DataFormat::RG11B10Float: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case DataFormat::RGBA8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
    case DataFormat::RGBA8UnormSrgb: return VK_FORMAT_R8G8B8A8_SRGB;
    case DataFormat::RGBA8Uint: return VK_FORMAT_R8G8B8A8_UINT;
    case DataFormat::RGBA8Snorm: return VK_FORMAT_R8G8B8A8_SNORM;
    case DataFormat::RGBA8Sint: return VK_FORMAT_R8G8B8A8_SINT;
    case DataFormat::RG16Float: return VK_FORMAT_R16G16_SFLOAT;
    case DataFormat::RG16Unorm: return VK_FORMAT_R16G16_UNORM;
    case DataFormat::RG16Uint: return VK_FORMAT_R16G16_UINT;
    case DataFormat::RG16Snorm: return VK_FORMAT_R16G16_SNORM;
    case DataFormat::RG16Sint: return VK_FORMAT_R16G16_SINT;
    case DataFormat::D32Float: return VK_FORMAT_D32_SFLOAT;
    case DataFormat::R32Float: return VK_FORMAT_R32_SFLOAT;
    case DataFormat::R32Uint: return VK_FORMAT_R32_UINT;
    case DataFormat::R32Sint: return VK_FORMAT_R32_SINT;
    case DataFormat::D24UnormS8Uint: return VK_FORMAT_D24_UNORM_S8_UINT;
    case DataFormat::RG8Unorm: return VK_FORMAT_R8G8_UNORM;
    case DataFormat::RG8Uint: return VK_FORMAT_R8G8_UINT;
    case DataFormat::RG8Snorm: return VK_FORMAT_R8G8_SNORM;
    case DataFormat::RG8Sint: return VK_FORMAT_R8G8_SINT;
    case DataFormat::R16Float: return VK_FORMAT_R16_SFLOAT;
    case DataFormat::D16Unorm: return VK_FORMAT_D16_UNORM;
    case DataFormat::R16Unorm: return VK_FORMAT_R16_UNORM;
    case DataFormat::R16Uint: return VK_FORMAT_R16_UINT;
    case DataFormat::R16Snorm: return VK_FORMAT_R16_SNORM;
    case DataFormat::R16Sint: return VK_FORMAT_R16_SINT;
    case DataFormat::R8Unorm: return VK_FORMAT_R8_UNORM;
    case DataFormat::R8Uint: return VK_FORMAT_R8_UINT;
    case DataFormat::R8Snorm: return VK_FORMAT_R8_SNORM;
    case DataFormat::R8Sint: return VK_FORMAT_R8_SINT;
    case DataFormat::RGB9E5UFloat: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    case DataFormat::RG8BG8Unorm: return VK_FORMAT_B8G8R8G8_422_UNORM;
    case DataFormat::GR8GB8Unorm: return VK_FORMAT_G8B8G8R8_422_UNORM;
    case DataFormat::BC1RGBAUnorm: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    case DataFormat::BC1RGBAUnormSrgb: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
    case DataFormat::BC2RGBAUnorm: return VK_FORMAT_BC2_UNORM_BLOCK;
    case DataFormat::BC2RGBAUnormSrgb: return VK_FORMAT_BC2_SRGB_BLOCK;
    case DataFormat::BC3RGBAUnorm: return VK_FORMAT_BC3_UNORM_BLOCK;
    case DataFormat::BC3RGBAUnormSrgb: return VK_FORMAT_BC3_SRGB_BLOCK;
    case DataFormat::BC4RUnorm: return VK_FORMAT_BC4_UNORM_BLOCK;
    case DataFormat::BC4RSnorm: return VK_FORMAT_BC4_SNORM_BLOCK;
    case DataFormat::BC5RGUnorm: return VK_FORMAT_BC5_UNORM_BLOCK;
    case DataFormat::BC5RGSnorm: return VK_FORMAT_BC5_SNORM_BLOCK;
    case DataFormat::B5R6G5Unorm: return VK_FORMAT_B5G6R5_UNORM_PACK16;
    case DataFormat::B5G5R5A1Unorm: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    case DataFormat::BGRA8Unorm: return VK_FORMAT_B8G8R8A8_UNORM;
    case DataFormat::BGRA8UnormSrgb: return VK_FORMAT_B8G8R8A8_SRGB;
    case DataFormat::BC6HUfloat16: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
    case DataFormat::BC6HSfloat16: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
    case DataFormat::BC7RGBAUnorm: return VK_FORMAT_BC7_UNORM_BLOCK;
    case DataFormat::BC7RGBAUnormSrgb: return VK_FORMAT_BC7_SRGB_BLOCK;
    }
}
}
