#pragma once
#include <wisdom/api/api_internal.h>
#include <wisdom/api/api_barrier.h>
#include <wisdom/vulkan/vk_resource.h>
#include <wisdom/vulkan/vk_format.h>
#include <wisdom/vulkan/vk_checks.h>
#include <span>


namespace wis
{
	class VKCommandList;

	template<>
	class Internal<VKCommandList>
	{
	public:
		Internal() = default;
		Internal(wis::shared_handle<vk::CommandPool> allocator, vk::CommandBuffer command_list)
			:allocator(std::move(allocator)), command_list(std::move(command_list)) {}
	public:
		//ID3D12GraphicsCommandList9* GetCommandList()const noexcept
		//{
		//	return command_list.get();
		//}
		//ID3D12CommandAllocator* GetCommandAllocator()const noexcept
		//{
		//	return allocator.get();
		//}
		//ID3D12PipelineState* GetBoundState()const noexcept
		//{
		//	return pipeline.get();
		//}
	protected:
		wis::shared_handle<vk::CommandPool> allocator;
		vk::CommandBuffer command_list;
		//winrt::com_ptr<ID3D12PipelineState> pipeline;
	};

	using VKCommandListView = vk::CommandBuffer;

	class VKCommandList : public QueryInternal<VKCommandList>
	{
	public:
		VKCommandList() = default;
		explicit VKCommandList(wis::shared_handle<vk::CommandPool> allocator, vk::CommandBuffer command_list)
			:QueryInternal(std::move(allocator), std::move(command_list))
		{

		}
		operator VKCommandListView()const noexcept
		{
			return command_list;
		}
	public:
		//void SetPipeline(DX12PipelineStateView xpipeline)noexcept
		//{
		//	pipeline.copy_from(xpipeline);
		//}
		bool Reset()noexcept
		{
			Close();

			vk::CommandBufferBeginInfo desc{};
			closed = false;
			command_list.begin(desc);
			return !closed;
		}
		[[nodiscard]] bool IsClosed()const noexcept
		{
			return closed;
		}
		bool Close()noexcept
		{
			if (closed)return closed;
			command_list.end();
			return closed = true;
		}


		void BufferBarrier(wis::BufferBarrier barrier, VKBufferView buffer)noexcept
		{
			auto acc_before = convert_vk(barrier.access_before);
			auto acc_after = convert_vk(barrier.access_after);

			if (!buffer || acc_before == acc_after)
				return;

			vk::BufferMemoryBarrier desc
			{
				acc_before, acc_after, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, buffer, 0, VK_WHOLE_SIZE
			};
			command_list.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands,
				vk::DependencyFlagBits::eByRegion,
				0, nullptr,
				1, &desc, 0, nullptr);
		}
		void TextureBarrier(wis::TextureBarrier barrier, VKTextureView texture)noexcept
		{
			vk::ImageLayout vk_state_before = convert_vk(barrier.state_before);
			vk::ImageLayout vk_state_after = convert_vk(barrier.state_after);

			auto acc_before = convert_vk(barrier.access_before);
			auto acc_after = convert_vk(barrier.access_after);

			if (!texture.image || (vk_state_before == vk_state_after && acc_before == acc_after))
				return;

			vk::ImageMemoryBarrier image_memory_barrier
			{
				acc_before,
					acc_after,
					vk_state_before,
					vk_state_after,
					VK_QUEUE_FAMILY_IGNORED,
					VK_QUEUE_FAMILY_IGNORED,
					texture.image,
					vk::ImageSubresourceRange{
						aspect_flags(texture.format),
						barrier.base_mip_level,
						barrier.level_count,
						barrier.base_array_layer,
						barrier.layer_count
					}
			};
			command_list.pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands,
				vk::DependencyFlagBits::eByRegion,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier);
		}


		//void ClearRenderTarget(DX12RenderTargetView rtv, std::span<const float, 4> color)noexcept
		//{
		//	command_list->ClearRenderTargetView(rtv.GetInternal().GetHandle(), color.data(), 0, nullptr);
		//}
		//
		void CopyBuffer(VKBufferView source, VKBufferView destination, size_t data_size)noexcept
		{
			vk::BufferCopy bufCopy{
				0, // srcOffset
				0, // dstOffset,
				data_size
			}; // size
			command_list.copyBuffer(source, destination, bufCopy);
		}
		//
		//void SetGraphicsRootSignature(DX12RootSignatureView root)noexcept
		//{
		//	command_list->SetGraphicsRootSignature(root);
		//}
		//
		void RSSetViewport(Viewport vp)noexcept
		{
			vk::Viewport viewport;
			viewport.x = vp.top_leftx;
			viewport.y = vp.top_lefty;
			viewport.width = vp.width;
			viewport.height = -vp.height;
			viewport.minDepth = vp.min_depth;
			viewport.maxDepth = vp.max_depth;
			command_list.setViewport(0, 1, &viewport);
		}
		void RSSetScissorRect(ScissorRect srect)noexcept
		{
			vk::Rect2D rect;
			rect.offset.x = srect.left;
			rect.offset.y = srect.top;
			rect.extent.width = srect.right;
			rect.extent.height = srect.bottom;
			command_list.setScissor(0, 1, &rect);
		}
		//void IASetPrimitiveTopology(PrimitiveTopology vp)noexcept
		//{
		//	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY(vp));
		//}
		//void IASetVertexBuffers(std::span<const DX12VertexBufferView> resources, uint32_t start_slot = 0)noexcept
		//{
		//	command_list->IASetVertexBuffers(start_slot, resources.size(), (const D3D12_VERTEX_BUFFER_VIEW*)resources.data());
		//}
		//
		//void OMSetRenderTargets(std::span<const DX12RenderTargetView> rtvs, void* dsv = nullptr)noexcept
		//{
		//	command_list->OMSetRenderTargets(uint32_t(rtvs.size()), (const D3D12_CPU_DESCRIPTOR_HANDLE*)(rtvs.data()), false, (D3D12_CPU_DESCRIPTOR_HANDLE*)dsv);
		//}
		//void DrawInstanced(uint32_t VertexCountPerInstance,
		//	uint32_t InstanceCount = 1,
		//	uint32_t StartVertexLocation = 0,
		//	uint32_t StartInstanceLocation = 0)noexcept
		//{
		//	command_list->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
		//}
	private:
		bool closed = true;
	};
}