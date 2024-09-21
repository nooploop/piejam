// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    ~on_scope_exit()
    {
        m_f();
    }

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
