#pragma once
#include <wisdom/api/api_internal.h>
#include <wisdom/api/api_shader.h>
#include <vector>
#include <span>

namespace wis
{
	class DX12Shader;

	template<>
	class Internal<DX12Shader>
	{
	public:
		std::span<const std::byte> GetShaderBytecode()const noexcept
		{
			return bytecode.GetSpan();
		}
	protected:
		wis::shared_blob<std::byte> bytecode;
	};

	class DX12Shader : public QueryInternal<DX12Shader>
	{
	public:
		using DataType = std::byte;
		static constexpr inline ShaderLang language = ShaderLang::dxil;
	public:
		DX12Shader() = default;
		explicit DX12Shader(std::shared_ptr<DataType[]> xbytecode, size_t size, ShaderType type)
			:type(type)
		{
			bytecode = { std::move(xbytecode), size };
		}
	public:
		operator bool()const noexcept
		{
			return type != ShaderType::unknown && !bytecode.empty();
		}
		auto GetType()const noexcept
		{
			return type;
		}
	public:
		ShaderType type = ShaderType::unknown;
	};
}