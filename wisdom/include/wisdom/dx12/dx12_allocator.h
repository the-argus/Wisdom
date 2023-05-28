#pragma once
#include <wisdom/dx12/dx12_device.h>
#include <D3D12MemAlloc.h>
#include <wisdom/api/api_barrier.h>

namespace wis
{
	class DX12ResourceAllocator;

	template<>
	class Internal<DX12ResourceAllocator>
	{
	public:
		D3D12MA::Allocator* GetAllocator()const noexcept
		{
			return allocator.get();
		}
	protected:
		winrt::com_ptr<D3D12MA::Allocator> allocator;
	};


	/// @brief Resource allocator for DX12
	class DX12ResourceAllocator : public QueryInternal<DX12ResourceAllocator>
	{
	public:
		DX12ResourceAllocator() = default;
		explicit DX12ResourceAllocator(DX12DeviceView device, DX12AdapterView adapter)
		{
			D3D12MA::ALLOCATOR_DESC desc{
				.Flags = D3D12MA::ALLOCATOR_FLAGS::ALLOCATOR_FLAG_NONE,
				.pDevice = device,
				.PreferredBlockSize = 0,
				.pAllocationCallbacks = nullptr,
				.pAdapter = adapter
			};
			wis::check_hresult(D3D12MA::CreateAllocator(&desc, allocator.put()));
		}
	public:
		/// @brief Create a buffer that is persistently mapped to the GPU
		/// @param size Size of the buffer
		/// @param flags Type of buffer
		/// @return Buffer object
		[[nodiscard]]
		DX12Buffer CreatePersistentBuffer(size_t size, [[maybe_unused]] BufferFlags flags = BufferFlags::None)
		{
			using namespace river::flags;
			winrt::com_ptr<ID3D12Resource> rc;
			winrt::com_ptr<D3D12MA::Allocation> al;
			auto desc = CD3DX12_RESOURCE_DESC1::Buffer(size);
			D3D12MA::ALLOCATION_DESC all_desc = {};
			all_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

			D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COPY_DEST;

			allocator->CreateResource2(&all_desc, &desc,
				state, nullptr,
				al.put(), __uuidof(*rc), rc.put_void());

			return DX12Buffer{ std::move(rc), std::move(al)};
		}

		/// @brief Create a buffer that is accessible by the CPU and serves as a staging buffer for GPU uploads
		/// @param size Size of the buffer
		/// @return Buffer object
		[[nodiscard]]
		DX12Buffer CreateUploadBuffer(size_t size)
		{
			winrt::com_ptr<ID3D12Resource> rc;
			winrt::com_ptr<D3D12MA::Allocation> al;
			auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);
			D3D12MA::ALLOCATION_DESC all_desc = {};
			all_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			allocator->CreateResource(&all_desc, &desc,
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
				al.put(), __uuidof(*rc), rc.put_void());

			return DX12Buffer{ std::move(rc), std::move(al)};
		}
	};
}