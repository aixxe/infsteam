#pragma once

namespace util
{
    auto find(std::span<std::uint8_t> region, std::string_view pattern) -> std::optional<std::uint8_t*>;
    auto rfind(std::span<std::uint8_t> region, std::string_view pattern) -> std::optional<std::uint8_t*>;

    auto get_displacement(auto rip, int offset)
        { return *reinterpret_cast<std::int32_t*>(rip + offset); }

    auto follow_relative(auto rip, int offset, int size)
        { return rip + get_displacement(rip, offset) + size; }
}