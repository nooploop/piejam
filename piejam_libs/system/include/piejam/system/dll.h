// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
    dll(std::filesystem::path const&);
    dll(dll&&) noexcept;
    dll(dll const&) = delete;
    ~dll();

    auto operator=(dll &&) -> dll&;
    auto operator=(dll const&) -> dll& = delete;

    operator bool() const noexcept { return m_handle; }

    template <class Signature>
    auto symbol(std::string_view const& name) const
    {
        return detail::dll_symbol<Signature>{}(symbol(name));
    }

private:
    auto symbol(std::string_view const& name) const -> void*;

    void* m_handle{};
};

} // namespace piejam::system
