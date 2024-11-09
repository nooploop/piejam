// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/functional/memo.h>

#include <boost/hof/combine.hpp>
#include <boost/hof/construct.hpp>

#include <concepts>
#include <functional>
#include <type_traits>

namespace piejam::redux
{

template <class Value, class State>
class selector
{
public:
    using value_type = Value;
    using state_type = State;
    using function_type = std::function<Value(State const&)>;

    selector() = delete;

    template <std::invocable<State const&> F>
        requires(!std::is_same_v<selector, std::decay_t<F>>)
    selector(F&& f)
        : m_f(std::forward<F>(f))
    {
    }

    [[nodiscard]]
    auto operator()(State const& st) const -> Value
    {
        return m_f(st);
    }

private:
    function_type m_f;
};

template <class Out, class... In, class State>
[[nodiscard]]
auto
combine(Out&& out, selector<In, State>... ins)
        -> selector<std::invoke_result_t<Out, In const&...>, State>
{
    return [get = boost::hof::combine(
                    boost::hof::construct<std::tuple>(),
                    std::move(ins)...),
            out = memo(std::forward<Out>(out))](State const& st) {
        return std::apply(out, get((((void)std::is_void_v<In>), st)...));
    };
}

} // namespace piejam::redux
