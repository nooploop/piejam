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

#include <concepts>
#include <type_traits>
#include <utility>

namespace piejam
{

template <class F>
struct on_scope_exit
{
    on_scope_exit(F&& f) noexcept(std::is_nothrow_move_constructible_v<F>)
        : m_f(std::move(f))
    {
    }

    ~on_scope_exit() { m_f(); }

    on_scope_exit(on_scope_exit const&) = delete;
    on_scope_exit(on_scope_exit&&) = delete;

    auto operator=(on_scope_exit const&) -> on_scope_exit& = delete;
    auto operator=(on_scope_exit&&) -> on_scope_exit& = delete;

private:
    F m_f;
};

template <class F>
on_scope_exit(F&&) -> on_scope_exit<F>;

} // namespace piejam
