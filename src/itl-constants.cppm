module;
#include <string_view>
export module iobz_containers:constants;

export namespace itl
{
    constexpr std::string_view OOB_ERR = "Out of Bounds Access!!!";
    constexpr std::string_view UNDERF_ERR = "Container Underflow!!!";
    constexpr std::string_view OVERF_ERR = "Container Overflow!!!";



    template<typename T>
    concept Hashable = requires(T type)
    {
        { std::hash<T>{}(type) } -> std::convertible_to<std::size_t>;
    };

    template<typename T>
    concept CopyAndHashable = Hashable<T> && std::copyable<T>;
}