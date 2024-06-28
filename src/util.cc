#include "util.h"

using namespace util;

auto find_generic(auto&& method, auto&& region, auto&& pattern) -> std::optional<std::uint8_t*>
{
    auto target = std::vector<std::optional<std::uint8_t>> {};

    for (auto&& range: pattern | std::views::split(' '))
    {
        auto hex = std::string_view { range };
        auto bin = std::optional<std::uint8_t> {};

        if (hex.empty())
            continue;

        if (!hex.starts_with('?'))
            bin = std::stoi(hex.data(), nullptr, 16);

        target.emplace_back(bin);
    }

    auto result = method(region, target, [] (auto a, auto b)
        { return !b || a == *b; });

    if (result.empty())
        return std::nullopt;

    return result.data();
}

auto util::find(std::span<std::uint8_t> region, std::string_view pattern) -> std::optional<std::uint8_t*>
    { return find_generic(std::ranges::search, region, pattern); }

auto util::rfind(std::span<std::uint8_t> region, std::string_view pattern) -> std::optional<std::uint8_t*>
    { return find_generic(std::ranges::find_end, region, pattern); }