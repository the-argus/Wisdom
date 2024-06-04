#pragma once
#include <string>
#include <memory>
#include <span>

namespace wis {
struct string_hash {
    using is_transparent = void;
    [[nodiscard]] size_t operator()(const char* txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(std::string_view txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(const std::string& txt) const
    {
        return std::hash<std::string>{}(txt);
    }
};
} // namespace wis

namespace wis::detail {
template<class Type, std::enable_if_t<std::is_unbounded_array_v<Type>, int> = 0>
[[nodiscard]] constexpr std::unique_ptr<Type> make_unique_for_overwrite(size_t size) noexcept
{
    // make a unique_ptr with default initialization
    using Elem = std::remove_extent_t<Type>;
    return std::unique_ptr<Type>(new (std::nothrow) Elem[size]);
}
template<class Type, class... Types, std::enable_if_t<!std::is_array_v<Type>, int> = 0>
[[nodiscard]] constexpr std::unique_ptr<Type> make_unique(Types&&... Args) noexcept
{ // make a unique_ptr
    return std::unique_ptr<Type>(new (std::nothrow) Type(std::forward<Types>(Args)...));
}

template<std::integral I>
constexpr inline I aligned_size(I size, I alignment) noexcept
{
    return (size + alignment - 1) & ~(alignment - 1);
}

template<typename T>
struct fixed_allocation {
    std::unique_ptr<T[]> data;
    size_t size = 0;

    constexpr operator std::span<const T>() const noexcept
    {
        return { data.get(), size };
    }
    constexpr operator bool() const noexcept
    {
        return data != nullptr;
    }
    constexpr auto* get() noexcept
    {
        return data.get();
    }
    constexpr const auto* get() const noexcept
    {
        return data.get();
    }
    constexpr auto& operator[](size_t index) noexcept
    {
        return data[index];
    }
    constexpr const auto& operator[](size_t index) const noexcept
    {
        return data[index];
    }
    constexpr auto begin() noexcept
    {
        return data.get();
    }
    constexpr auto end() noexcept
    {
        return data.get() + size;
    }
    constexpr auto begin() const noexcept
    {
        return data.get();
    }
    constexpr auto end() const noexcept
    {
        return data.get() + size;
    }

    constexpr auto* get_data() noexcept
    {
        return data.get();
    }
    constexpr const auto* get_data() const noexcept
    {
        return data.get();
    }
};

template<typename T>
[[nodiscard]] constexpr fixed_allocation<T> make_fixed_allocation(size_t size) noexcept
{
    return { make_unique_for_overwrite<T[]>(size), size };
}
} // namespace wis::detail
