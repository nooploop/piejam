// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>
#include <functional>
#include <string_view>
#include <type_traits>

namespace piejam::system
{

namespace detail
{

template <class>
struct dll_symbol;

template <class R, class... Args>
struct dll_symbol<R(Args...)>
{
    auto operator()(void* const sym) const noexcept -> R (*)(Args...)
    {
        return reinterpret_cast<R (*)(Args...)>(sym);
    }
};

template <class R, class... Args>
struct dll_symbol<R (*)(Args...)>
{
    auto operator()(void* const sym) const noexcept -> R (*)(Args...)
    {
        return reinterpret_cast<R (*)(Args...)>(sym);
    }
};

} // namespace detail

class dll
{
public:
    constexpr dll() noexcept = default;
    explicit dll(std::filesystem::path const&);
    dll(dll&&) noexcept;
    dll(dll const&) = delete;
    ~dll();

    auto operator=(dll&&) -> dll&;
    auto operator=(dll const&) -> dll& = delete;

    [[nodiscard]] operator bool() const noexcept
    {
        return m_handle;
    }

    template <class Signature>
    auto symbol(std::string_view const name) const
    {
        return detail::dll_symbol<Signature>{}(symbol(name));
    }

private:
    [[nodiscard]]
    auto symbol(std::string_view const name) const -> void*;

    void* m_handle{};
};

} // namespace piejam::system
