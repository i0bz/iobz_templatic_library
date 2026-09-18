module;
#include <string_view>
export module iobz_containers:constants;

namespace itl
{
    constexpr std::string_view OOB_ERR = "Out of Bounds Access!!!";
    constexpr std::string_view UNDERF_ERR = "Container Underflow!!!";
    constexpr std::string_view OVERF_ERR = "Container Overflow!!!";
}