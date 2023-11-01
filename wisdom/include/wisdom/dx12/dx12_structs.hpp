// Generated by wisdom generator on 2023-11-01 18:47:22.0161697 GMT+1
#pragma once
#include <wisdom/dx12/xdx12_views.h>

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
    wis::DX12GraphicsShaderStages shaders;
};

}
