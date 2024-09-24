// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>
#include <functional>
#include <type_traits>

namespace piejam::reselect
{

template <class Value, class State>
class selector
{
public:
    using value_type = Value;
    using state_type = State;
    using function_type = std::function<Value(State const&)>;

    template <std::invocable<State const&> F>
    selector(F&& f)
        : m_f(std::forward<F>(f))
    {
    }

    [[nodiscard]]
    auto get(State const& st) const -> Value
    {
        return m_f(st);
    }

private:
    function_type m_f;
};

} // namespace piejam::reselect
