#pragma once
#ifndef WISDOM_MODULES
#include <wisdom/api/api_internal.h>
#include <wisdom/vulkan/vk_views.h>
#endif

WIS_EXPORT namespace wis
{
    class VKRootSignature;

    template<>
    class Internal<VKRootSignature>
    {
    public:
        wis::shared_handle<vk::PipelineLayout> root;
    };

    /// @brief Root signature
    class VKRootSignature : public QueryInternal<VKRootSignature>
    {
    public:
        VKRootSignature() = default;
        explicit VKRootSignature(wis::shared_handle<vk::PipelineLayout> root)
            : QueryInternal(std::move(root))
        {
        }

        operator VKRootSignatureView() const noexcept
        {
            return root.get();
        }
    };
}
