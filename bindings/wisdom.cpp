#include "wisdom.h"

#if defined(WISDOM_DX12)
#include <wisdom/wisdom_dx12.hpp>

extern "C" DX12FenceView AsDX12FenceView(DX12Fence self)
{
    wis::DX12FenceView xself = reinterpret_cast<wis::DX12Fence&>(*self);
    return reinterpret_cast<DX12FenceView&>(xself);
}
extern "C" DX12BufferView AsDX12BufferView(DX12Buffer self)
{
    wis::DX12BufferView xself = reinterpret_cast<wis::DX12Buffer&>(*self);
    return reinterpret_cast<DX12BufferView&>(xself);
}
extern "C" DX12TextureView AsDX12TextureView(DX12Texture self)
{
    wis::DX12TextureView xself = reinterpret_cast<wis::DX12Texture&>(*self);
    return reinterpret_cast<DX12TextureView&>(xself);
}
extern "C" DX12RenderTargetView AsDX12RenderTargetView(DX12RenderTarget self)
{
    wis::DX12RenderTargetView xself = reinterpret_cast<wis::DX12RenderTarget&>(*self);
    return reinterpret_cast<DX12RenderTargetView&>(xself);
}
extern "C" DX12CommandListView AsDX12CommandListView(DX12CommandList self)
{
    wis::DX12CommandListView xself = reinterpret_cast<wis::DX12CommandList&>(*self);
    return reinterpret_cast<DX12CommandListView&>(xself);
}
extern "C" DX12ShaderView AsDX12ShaderView(DX12Shader self)
{
    wis::DX12ShaderView xself = reinterpret_cast<wis::DX12Shader&>(*self);
    return reinterpret_cast<DX12ShaderView&>(xself);
}
extern "C" DX12RootSignatureView AsDX12RootSignatureView(DX12RootSignature self)
{
    wis::DX12RootSignatureView xself = reinterpret_cast<wis::DX12RootSignature&>(*self);
    return reinterpret_cast<DX12RootSignatureView&>(xself);
}
extern "C" DX12DescriptorBufferView AsDX12DescriptorBufferView(DX12DescriptorBuffer self)
{
    wis::DX12DescriptorBufferView xself = reinterpret_cast<wis::DX12DescriptorBuffer&>(*self);
    return reinterpret_cast<DX12DescriptorBufferView&>(xself);
}
#endif

#if defined(WISDOM_VULKAN)
#include <wisdom/wisdom_vk.hpp>

extern "C" VKFenceView AsVKFenceView(VKFence self)
{
    wis::VKFenceView xself = reinterpret_cast<wis::VKFence&>(*self);
    return reinterpret_cast<VKFenceView&>(xself);
}
extern "C" VKBufferView AsVKBufferView(VKBuffer self)
{
    wis::VKBufferView xself = reinterpret_cast<wis::VKBuffer&>(*self);
    return reinterpret_cast<VKBufferView&>(xself);
}
extern "C" VKTextureView AsVKTextureView(VKTexture self)
{
    wis::VKTextureView xself = reinterpret_cast<wis::VKTexture&>(*self);
    return reinterpret_cast<VKTextureView&>(xself);
}
extern "C" VKRenderTargetView AsVKRenderTargetView(VKRenderTarget self)
{
    wis::VKRenderTargetView xself = reinterpret_cast<wis::VKRenderTarget&>(*self);
    return reinterpret_cast<VKRenderTargetView&>(xself);
}
extern "C" VKCommandListView AsVKCommandListView(VKCommandList self)
{
    wis::VKCommandListView xself = reinterpret_cast<wis::VKCommandList&>(*self);
    return reinterpret_cast<VKCommandListView&>(xself);
}
extern "C" VKShaderView AsVKShaderView(VKShader self)
{
    wis::VKShaderView xself = reinterpret_cast<wis::VKShader&>(*self);
    return reinterpret_cast<VKShaderView&>(xself);
}
extern "C" VKRootSignatureView AsVKRootSignatureView(VKRootSignature self)
{
    wis::VKRootSignatureView xself = reinterpret_cast<wis::VKRootSignature&>(*self);
    return reinterpret_cast<VKRootSignatureView&>(xself);
}
extern "C" VKDescriptorBufferView AsVKDescriptorBufferView(VKDescriptorBuffer self)
{
    wis::VKDescriptorBufferView xself = reinterpret_cast<wis::VKDescriptorBuffer&>(*self);
    return reinterpret_cast<VKDescriptorBufferView&>(xself);
}
#endif
