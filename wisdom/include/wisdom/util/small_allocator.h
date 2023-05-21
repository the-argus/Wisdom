#pragma once
#include <cassert>
#include <array>
#include <bitset>

namespace wis
{
	inline constexpr auto allocator_size = 1024u;

	template<size_t max_size = allocator_size>
	class stack_allocator
	{
	public:
		stack_allocator() = default;
		stack_allocator(const stack_allocator&) = delete;
		stack_allocator(stack_allocator&&) = delete;
	public:
		template<typename T> requires std::is_trivially_destructible_v<T>
		T& allocate()noexcept
		{
			T& x = *reinterpret_cast<T*>(allocator.data() + rsize);
			rsize += sizeof(T);
			assert(rsize <= max_size);
			return x;
		};
		template<typename T>
		T* get() noexcept
		{
			return reinterpret_cast<T*>(allocator.data());
		}

		size_t size_bytes()const noexcept
		{
			return rsize;
		}
	private:
		alignas(void*)std::array<std::byte, max_size> allocator{};
		size_t rsize = 0;
	};

	template<class T, size_t max_size = 16> requires std::is_trivially_destructible_v<T>
	class uniform_allocator
	{
	public:
		uniform_allocator() = default;
	public:
		template<typename ...Args>
		T& allocate(Args&&... args)noexcept
		{
			T* x = new(allocator.data() + rsize)T(std::forward<Args>(args)...);
			rsize++;
			assert(rsize <= max_size);
			return *x;
		};
		const T* get()const noexcept
		{
			return allocator.data();
		}
		const T* data()const noexcept
		{
			return allocator.data();
		}
		decltype(auto) at(size_t n)noexcept
		{
			return allocator.at(n);
		}
		decltype(auto) at(size_t n)const noexcept
		{
			return allocator.at(n);
		}

		size_t size()const noexcept
		{
			return rsize;
		}
		void compress_free(std::bitset<max_size> map)noexcept
		{
			for (size_t i = rsize; i < max_size; i++)
				if (map[i])allocator[rsize++] = allocator[i];
		}

		decltype(auto) begin()
		{
			return allocator.begin();
		}
		decltype(auto) begin()const
		{
			return allocator.begin();
		}

		decltype(auto) end()
		{
			return allocator.begin() + rsize;
		}
		decltype(auto) end()const
		{
			return allocator.begin() + rsize;
		}
	private:
		alignas(void*)std::array<T, max_size> allocator{};
		size_t rsize = 0;
	};
}