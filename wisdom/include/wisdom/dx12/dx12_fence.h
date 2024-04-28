#pragma once
#include <d3d12.h>
#include <wisdom/global/internal.h>
#include <wisdom/util/com_ptr.h>
#include <wisdom/dx12/dx12_checks.h>
#include <wisdom/dx12/dx12_views.h>

namespace wis {
class DX12Fence;

struct unique_event {
    unique_event() noexcept
        : hevent(CreateEventW(nullptr, false, false, nullptr)) { }
    unique_event(unique_event const&) = delete;
    unique_event& operator=(unique_event const&) = delete;
    unique_event(unique_event&& o) noexcept
        : hevent(std::exchange(o.hevent, nullptr)) { }
    unique_event& operator=(unique_event&& o) noexcept
    {
        std::swap(hevent, o.hevent);
        return *this;
    }
    ~unique_event() noexcept
    {
        if (hevent)
            CloseHandle(hevent);
    }
    auto get() const noexcept
    {
        return hevent;
    }
    operator bool() const noexcept
    {
        return bool(hevent);
    }
    wis::Status wait(uint32_t wait_ms) const noexcept
    {
        auto st = WaitForSingleObject(hevent, wait_ms);
        if (st == WAIT_OBJECT_0)
            return wis::Status::Ok;
        if (st == WAIT_TIMEOUT)
            return wis::Status::Timeout;
        return wis::Status::Error;
    }

public:
    HANDLE hevent;
};

template<>
class Internal<DX12Fence>
{
public:
    wis::com_ptr<ID3D12Fence1> fence;
    unique_event fence_event;
};

/// @brief A fence is a synchronization primitive that allows the CPU to wait for the GPU to finish
/// rendering a frame.
class DX12Fence : public QueryInternal<DX12Fence>
{
public:
    DX12Fence() = default;
    explicit DX12Fence(wis::com_ptr<ID3D12Fence1> xfence) noexcept
        : QueryInternal(std::move(xfence)) { }
    DX12Fence(DX12Fence&& o) noexcept = default;
    DX12Fence& operator=(DX12Fence&& o) noexcept = default;

    operator DX12FenceView() const noexcept
    {
        return fence.get();
    }

    operator bool() const noexcept
    {
        return bool(fence);
    }

public:
    /// @brief Get the current value of the fence.
    /// @return Value of the fence.
    [[nodiscard]] uint64_t
    GetCompletedValue() const noexcept
    {
        return fence->GetCompletedValue();
    }

    /// @brief Wait for the fence to reach a certain value.
    /// @param value Value to wait for.
    /// @return Boolean indicating whether the fence reached the value.
    [[nodiscard]] WIS_INLINE wis::Result
    Wait(uint64_t value,
         uint64_t wait_ns = std::numeric_limits<uint64_t>::max()) const noexcept;

    /// @brief Signal the fence from CPU.
    /// @param value Value to signal.
    [[nodiscard]] WIS_INLINE wis::Result Signal(uint64_t value) const noexcept
    {
        HRESULT hr = fence->Signal(value);
        return !succeeded(hr) ? wis::make_result<FUNC, "Failed to signal fence">(hr) : wis::success;
    }
};
} // namespace wis

#ifdef WISDOM_HEADER_ONLY
#include "impl/dx12_fence.cpp"
#endif // !WISDOM_HEADER_ONLY
