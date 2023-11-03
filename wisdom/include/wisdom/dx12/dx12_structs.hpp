// Generated by wisdom generator on 2023-11-03 13:38:30.5432961 GMT+1
#pragma once
#include <wisdom/dx12/xdx12_views.h>
#include <wisdom/api/api.h>

namespace wis{
struct DX12GraphicsShaderStages{
    wis::DX12ShaderView vertex;
    wis::DX12ShaderView hull;
    wis::DX12ShaderView domain;
    wis::DX12ShaderView geometry;
    wis::DX12ShaderView pixel;
};

struct DX12GraphicsPipelineDesc{
    wis::DX12RootSignatureView root_signature;
    wis::InputLayout input_layout;
    wis::DX12GraphicsShaderStages shaders;
};

inline constexpr DXGI_GPU_PREFERENCE convert_dx(AdapterPreference value) noexcept{
    switch(value){
    default: return {};
    case AdapterPreference::None: return DXGI_GPU_PREFERENCE_UNSPECIFIED;
    case AdapterPreference::MinConsumption: return DXGI_GPU_PREFERENCE_MINIMUM_POWER;
    case AdapterPreference::Performance: return DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    }
}
inline constexpr D3D12_SHADER_VISIBILITY convert_dx(ShaderStages value) noexcept {
    return static_cast<D3D12_SHADER_VISIBILITY>(value);
}
inline constexpr DXGI_FORMAT convert_dx(DataFormat value) noexcept {
    return static_cast<DXGI_FORMAT>(value);
}
}
