// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/tuple.h>

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace piejam
{

template <class F>
class memo
{
public:
    explicit memo(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
        : m_f(std::move(f))
    {
    }

    template <class... Args>
    auto operator()(Args&&... args) const -> std::add_lvalue_reference_t<
            std::add_const_t<std::invoke_result_t<F, Args&&...>>>
    {
        if (!m_last ||
            m_last->args != std::forward_as_tuple(std::forward<Args>(args)...))
        {
            auto result = std::invoke(m_f, std::forward<Args>(args)...);
            m_last.emplace(
                    std::tuple<Args...>(std::forward<Args>(args)...),
                    std::move(result));
        }

        return m_last->result;
    }

private:
    struct args_and_result
    {
        using args_tuple_type =
                tuple::decay_elements_t<boost::callable_traits::args_t<F>>;
        using result_type =
                std::decay_t<boost::callable_traits::return_type_t<F>>;

        template <class Args, class Result>
        args_and_result(Args&& args, Result&& result)
            : args(std::forward<Args>(args))
            , result(std::forward<Result>(result))
        {
        }

        args_tuple_type args;
        result_type result;
    };

    F m_f;

    mutable std::optional<args_and_result> m_last;
};

template <class F>
memo(F) -> memo<F>;

} // namespace piejam
