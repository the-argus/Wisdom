// Generated by wisdom generator on 2024-02-14 14:52:13.0381897 GMT+1
#include "wisdom.h"
#include <wisdom/wisdom.hpp>

DX12CommandListView AsDX12CommandListView(DX12CommandList self)
{
    return reinterpret_cast<DX12CommandListView&>(static_cast<wis::DX12CommandListView>(reinterpret_cast<wis::DX12CommandList&>(*self)));
}
VKCommandListView AsVKCommandListView(VKCommandList self)
{
    return reinterpret_cast<VKCommandListView&>(static_cast<wis::VKCommandListView>(reinterpret_cast<wis::VKCommandList&>(*self)));
}
DX12FenceView AsDX12FenceView(DX12Fence self)
{
    return reinterpret_cast<DX12FenceView&>(static_cast<wis::DX12FenceView>(reinterpret_cast<wis::DX12Fence&>(*self)));
}
VKFenceView AsVKFenceView(VKFence self)
{
    return reinterpret_cast<VKFenceView&>(static_cast<wis::VKFenceView>(reinterpret_cast<wis::VKFence&>(*self)));
}
DX12ShaderView AsDX12ShaderView(DX12Shader self)
{
    return reinterpret_cast<DX12ShaderView&>(static_cast<wis::DX12ShaderView>(reinterpret_cast<wis::DX12Shader&>(*self)));
}
VKShaderView AsVKShaderView(VKShader self)
{
    return reinterpret_cast<VKShaderView&>(static_cast<wis::VKShaderView>(reinterpret_cast<wis::VKShader&>(*self)));
}
DX12RootSignatureView AsDX12RootSignatureView(DX12RootSignature self)
{
    return reinterpret_cast<DX12RootSignatureView&>(static_cast<wis::DX12RootSignatureView>(reinterpret_cast<wis::DX12RootSignature&>(*self)));
}
VKRootSignatureView AsVKRootSignatureView(VKRootSignature self)
{
    return reinterpret_cast<VKRootSignatureView&>(static_cast<wis::VKRootSignatureView>(reinterpret_cast<wis::VKRootSignature&>(*self)));
}
WisResult DX12CreateFactory( bool debug_layer,  DebugCallback callback,  void* user_data, DX12Factory* out_factory)
{
    auto&& ret = wis::DX12CreateFactory(debug_layer, reinterpret_cast<DebugCallback>(callback), user_data);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_factory = ok ? reinterpret_cast<DX12Factory>(new wis::DX12Factory(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12Factory>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateFactory( bool debug_layer,  DebugCallback callback,  void* user_data, VKFactory* out_factory)
{
    auto&& ret = wis::VKCreateFactory(debug_layer, reinterpret_cast<DebugCallback>(callback), user_data);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_factory = ok ? reinterpret_cast<VKFactory>(new wis::VKFactory(std::move(std::get<1>(ret)))) : reinterpret_cast<VKFactory>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12CreateDevice( DX12Factory factory,  DX12Adapter adapter, DX12Device* out_device)
{
    auto&& ret = wis::DX12CreateDevice(*reinterpret_cast<wis::Factory*>(factory), *reinterpret_cast<wis::Adapter*>(adapter));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_device = ok ? reinterpret_cast<DX12Device>(new wis::DX12Device(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12Device>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateDevice( VKFactory factory,  VKAdapter adapter, VKDevice* out_device)
{
    auto&& ret = wis::VKCreateDevice(*reinterpret_cast<wis::Factory*>(factory), *reinterpret_cast<wis::Adapter*>(adapter));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_device = ok ? reinterpret_cast<VKDevice>(new wis::VKDevice(std::move(std::get<1>(ret)))) : reinterpret_cast<VKDevice>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12GetAdapter(DX12Factory self,  uint32_t index,  WisAdapterPreference preference, DX12Adapter* out_adapter)
{
    auto* xself = reinterpret_cast<wis::DX12Factory*>(self);
    auto&& ret = xself->GetAdapter(index, reinterpret_cast<wis::AdapterPreference>(preference));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_adapter = ok ? reinterpret_cast<DX12Adapter>(new wis::DX12Adapter(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12Adapter>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKGetAdapter(VKFactory self,  uint32_t index,  WisAdapterPreference preference, VKAdapter* out_adapter)
{
    auto* xself = reinterpret_cast<wis::VKFactory*>(self);
    auto&& ret = xself->GetAdapter(index, reinterpret_cast<wis::AdapterPreference>(preference));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_adapter = ok ? reinterpret_cast<VKAdapter>(new wis::VKAdapter(std::move(std::get<1>(ret)))) : reinterpret_cast<VKAdapter>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
void DX12FactoryDestroy(DX12Factory self)
{
    auto* xself = reinterpret_cast<wis::DX12Factory*>(self);
    delete xself;
}
void VKFactoryDestroy(VKFactory self)
{
    auto* xself = reinterpret_cast<wis::VKFactory*>(self);
    delete xself;
}
WisResult DX12GetDesc(DX12Adapter self,  WisAdapterDesc* desc)
{
    auto* xself = reinterpret_cast<wis::DX12Adapter*>(self);
    auto&& ret = xself->GetDesc(reinterpret_cast<wis::AdapterDesc*>(desc));
    return reinterpret_cast<WisResult&>(ret);
}
WisResult VKGetDesc(VKAdapter self,  WisAdapterDesc* desc)
{
    auto* xself = reinterpret_cast<wis::VKAdapter*>(self);
    auto&& ret = xself->GetDesc(reinterpret_cast<wis::AdapterDesc*>(desc));
    return reinterpret_cast<WisResult&>(ret);
}
void DX12DeviceDestroy(DX12Device self)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    delete xself;
}
void VKDeviceDestroy(VKDevice self)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    delete xself;
}
WisResult DX12CreateFence(DX12Device self,  uint64_t initial_value, DX12Fence* out_fence)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->CreateFence(initial_value);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_fence = ok ? reinterpret_cast<DX12Fence>(new wis::DX12Fence(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12Fence>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateFence(VKDevice self,  uint64_t initial_value, VKFence* out_fence)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->CreateFence(initial_value);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_fence = ok ? reinterpret_cast<VKFence>(new wis::VKFence(std::move(std::get<1>(ret)))) : reinterpret_cast<VKFence>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12CreateRootSignature(DX12Device self,  WisRootConstant* constants,  uint32_t constants_size, DX12RootSignature* out_root_signature)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->CreateRootSignature(reinterpret_cast<wis::RootConstant*>(constants), constants_size);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_root_signature = ok ? reinterpret_cast<DX12RootSignature>(new wis::DX12RootSignature(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12RootSignature>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateRootSignature(VKDevice self,  WisRootConstant* constants,  uint32_t constants_size, VKRootSignature* out_root_signature)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->CreateRootSignature(reinterpret_cast<wis::RootConstant*>(constants), constants_size);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_root_signature = ok ? reinterpret_cast<VKRootSignature>(new wis::VKRootSignature(std::move(std::get<1>(ret)))) : reinterpret_cast<VKRootSignature>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12CreateAllocator(DX12Device self, DX12ResourceAllocator* out_allocator)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->CreateAllocator();
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_allocator = ok ? reinterpret_cast<DX12ResourceAllocator>(new wis::DX12ResourceAllocator(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12ResourceAllocator>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateAllocator(VKDevice self, VKResourceAllocator* out_allocator)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->CreateAllocator();
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_allocator = ok ? reinterpret_cast<VKResourceAllocator>(new wis::VKResourceAllocator(std::move(std::get<1>(ret)))) : reinterpret_cast<VKResourceAllocator>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12CreateCommandQueue(DX12Device self,  WisQueueType type,  WisQueuePriority priority, DX12CommandQueue* out_queue)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->CreateCommandQueue(reinterpret_cast<wis::QueueType>(type), reinterpret_cast<wis::QueuePriority>(priority));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_queue = ok ? reinterpret_cast<DX12CommandQueue>(new wis::DX12CommandQueue(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12CommandQueue>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateCommandQueue(VKDevice self,  WisQueueType type,  WisQueuePriority priority, VKCommandQueue* out_queue)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->CreateCommandQueue(reinterpret_cast<wis::QueueType>(type), reinterpret_cast<wis::QueuePriority>(priority));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_queue = ok ? reinterpret_cast<VKCommandQueue>(new wis::VKCommandQueue(std::move(std::get<1>(ret)))) : reinterpret_cast<VKCommandQueue>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12CreateShader(DX12Device self,  void* data,  uint32_t size_bytes, DX12Shader* out_shader)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->CreateShader(data, size_bytes);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_shader = ok ? reinterpret_cast<DX12Shader>(new wis::DX12Shader(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12Shader>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateShader(VKDevice self,  void* data,  uint32_t size_bytes, VKShader* out_shader)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->CreateShader(data, size_bytes);
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_shader = ok ? reinterpret_cast<VKShader>(new wis::VKShader(std::move(std::get<1>(ret)))) : reinterpret_cast<VKShader>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12WaitForMultipleFences(DX12Device self,  DX12FenceView* fences,  uint64_t* values,  uint32_t count,  WisMutiWaitFlags wait_all,  uint64_t timeout)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->WaitForMultipleFences(reinterpret_cast<wis::FenceView*>(fences), values, count, reinterpret_cast<wis::MutiWaitFlags>(wait_all), timeout);
    return reinterpret_cast<WisResult&>(ret);
}
WisResult VKWaitForMultipleFences(VKDevice self,  VKFenceView* fences,  uint64_t* values,  uint32_t count,  WisMutiWaitFlags wait_all,  uint64_t timeout)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->WaitForMultipleFences(reinterpret_cast<wis::FenceView*>(fences), values, count, reinterpret_cast<wis::MutiWaitFlags>(wait_all), timeout);
    return reinterpret_cast<WisResult&>(ret);
}
WisResult DX12CreatePipelineState(DX12Device self,  DX12GraphicsPipelineDesc* desc, DX12PipelineState* out_pipeline_state)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->CreatePipelineState(reinterpret_cast<wis::GraphicsPipelineDesc*>(desc));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_pipeline_state = ok ? reinterpret_cast<DX12PipelineState>(new wis::DX12PipelineState(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12PipelineState>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreatePipelineState(VKDevice self,  VKGraphicsPipelineDesc* desc, VKPipelineState* out_pipeline_state)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->CreatePipelineState(reinterpret_cast<wis::GraphicsPipelineDesc*>(desc));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_pipeline_state = ok ? reinterpret_cast<VKPipelineState>(new wis::VKPipelineState(std::move(std::get<1>(ret)))) : reinterpret_cast<VKPipelineState>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult DX12CreateCommandList(DX12Device self,  WisQueueType type, DX12CommandList* out_command_list)
{
    auto* xself = reinterpret_cast<wis::DX12Device*>(self);
    auto&& ret = xself->CreateCommandList(reinterpret_cast<wis::QueueType>(type));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_command_list = ok ? reinterpret_cast<DX12CommandList>(new wis::DX12CommandList(std::move(std::get<1>(ret)))) : reinterpret_cast<DX12CommandList>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
WisResult VKCreateCommandList(VKDevice self,  WisQueueType type, VKCommandList* out_command_list)
{
    auto* xself = reinterpret_cast<wis::VKDevice*>(self);
    auto&& ret = xself->CreateCommandList(reinterpret_cast<wis::QueueType>(type));
    bool ok = std::get<0>(ret).status == wis::Status::Success;
    *out_command_list = ok ? reinterpret_cast<VKCommandList>(new wis::VKCommandList(std::move(std::get<1>(ret)))) : reinterpret_cast<VKCommandList>(nullptr);
    return reinterpret_cast<WisResult&>(std::get<0>(ret));
}
void DX12RootSignatureDestroy(DX12RootSignature self)
{
    auto* xself = reinterpret_cast<wis::DX12RootSignature*>(self);
    delete xself;
}
void VKRootSignatureDestroy(VKRootSignature self)
{
    auto* xself = reinterpret_cast<wis::VKRootSignature*>(self);
    delete xself;
}
void DX12FenceDestroy(DX12Fence self)
{
    auto* xself = reinterpret_cast<wis::DX12Fence*>(self);
    delete xself;
}
void VKFenceDestroy(VKFence self)
{
    auto* xself = reinterpret_cast<wis::VKFence*>(self);
    delete xself;
}
uint64_t DX12GetCompletedValue(DX12Fence self)
{
    auto* xself = reinterpret_cast<wis::DX12Fence*>(self);
    auto&& ret = xself->GetCompletedValue();
    return reinterpret_cast<uint64_t&>(ret);
}
uint64_t VKGetCompletedValue(VKFence self)
{
    auto* xself = reinterpret_cast<wis::VKFence*>(self);
    auto&& ret = xself->GetCompletedValue();
    return reinterpret_cast<uint64_t&>(ret);
}
WisResult DX12Wait(DX12Fence self,  uint64_t value,  uint64_t timeout_ns)
{
    auto* xself = reinterpret_cast<wis::DX12Fence*>(self);
    auto&& ret = xself->Wait(value, timeout_ns);
    return reinterpret_cast<WisResult&>(ret);
}
WisResult VKWait(VKFence self,  uint64_t value,  uint64_t timeout_ns)
{
    auto* xself = reinterpret_cast<wis::VKFence*>(self);
    auto&& ret = xself->Wait(value, timeout_ns);
    return reinterpret_cast<WisResult&>(ret);
}
WisResult DX12Signal(DX12Fence self,  uint64_t value)
{
    auto* xself = reinterpret_cast<wis::DX12Fence*>(self);
    auto&& ret = xself->Signal(value);
    return reinterpret_cast<WisResult&>(ret);
}
WisResult VKSignal(VKFence self,  uint64_t value)
{
    auto* xself = reinterpret_cast<wis::VKFence*>(self);
    auto&& ret = xself->Signal(value);
    return reinterpret_cast<WisResult&>(ret);
}
void DX12ResourceAllocatorDestroy(DX12ResourceAllocator self)
{
    auto* xself = reinterpret_cast<wis::DX12ResourceAllocator*>(self);
    delete xself;
}
void VKResourceAllocatorDestroy(VKResourceAllocator self)
{
    auto* xself = reinterpret_cast<wis::VKResourceAllocator*>(self);
    delete xself;
}
void DX12CommandQueueDestroy(DX12CommandQueue self)
{
    auto* xself = reinterpret_cast<wis::DX12CommandQueue*>(self);
    delete xself;
}
void VKCommandQueueDestroy(VKCommandQueue self)
{
    auto* xself = reinterpret_cast<wis::VKCommandQueue*>(self);
    delete xself;
}
WisResult DX12SignalQueue(DX12CommandQueue self,  DX12FenceView fence,  uint64_t value)
{
    auto* xself = reinterpret_cast<wis::DX12CommandQueue*>(self);
    auto&& ret = xself->SignalQueue(reinterpret_cast<wis::FenceView>(fence), value);
    return reinterpret_cast<WisResult&>(ret);
}
WisResult VKSignalQueue(VKCommandQueue self,  VKFenceView fence,  uint64_t value)
{
    auto* xself = reinterpret_cast<wis::VKCommandQueue*>(self);
    auto&& ret = xself->SignalQueue(reinterpret_cast<wis::FenceView>(fence), value);
    return reinterpret_cast<WisResult&>(ret);
}
void DX12ExecuteCommandLists(DX12CommandQueue self,  DX12CommandListView* lists,  uint32_t count)
{
    auto* xself = reinterpret_cast<wis::DX12CommandQueue*>(self);
xself->ExecuteCommandLists(reinterpret_cast<wis::CommandListView*>(lists), count);
}
void VKExecuteCommandLists(VKCommandQueue self,  VKCommandListView* lists,  uint32_t count)
{
    auto* xself = reinterpret_cast<wis::VKCommandQueue*>(self);
xself->ExecuteCommandLists(reinterpret_cast<wis::CommandListView*>(lists), count);
}
void DX12ShaderDestroy(DX12Shader self)
{
    auto* xself = reinterpret_cast<wis::DX12Shader*>(self);
    delete xself;
}
void VKShaderDestroy(VKShader self)
{
    auto* xself = reinterpret_cast<wis::VKShader*>(self);
    delete xself;
}
void DX12PipelineStateDestroy(DX12PipelineState self)
{
    auto* xself = reinterpret_cast<wis::DX12PipelineState*>(self);
    delete xself;
}
void VKPipelineStateDestroy(VKPipelineState self)
{
    auto* xself = reinterpret_cast<wis::VKPipelineState*>(self);
    delete xself;
}
void DX12CommandListDestroy(DX12CommandList self)
{
    auto* xself = reinterpret_cast<wis::DX12CommandList*>(self);
    delete xself;
}
void VKCommandListDestroy(VKCommandList self)
{
    auto* xself = reinterpret_cast<wis::VKCommandList*>(self);
    delete xself;
}
void DX12BufferDestroy(DX12Buffer self)
{
    auto* xself = reinterpret_cast<wis::DX12Buffer*>(self);
    delete xself;
}
void VKBufferDestroy(VKBuffer self)
{
    auto* xself = reinterpret_cast<wis::VKBuffer*>(self);
    delete xself;
}
void SwapchainDestroy()
{
    auto* xself = reinterpret_cast<*>(self);
    delete xself;
}
