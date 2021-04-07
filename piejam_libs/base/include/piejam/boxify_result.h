// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include <functional>
#include <tuple>

namespace piejam
{

namespace detail
{

template <class F, class R, class Args>
struct with_boxed_result;

template <class F, class R, class... Args>
struct with_boxed_result<F, R, std::tuple<Args...>>
{
    F f;

    auto operator()(Args... args) const -> piejam::box<R>
    {
        return {std::invoke(f, args...)};
    }
};

} // namespace detail

template <class F>
auto
boxify_result(F&& f)
{
    using R = boost::callable_traits::return_type_t<F>;
    using Args = boost::callable_traits::args_t<F>;
    return detail::with_boxed_result<F, R, Args>{std::forward<F>(f)};
}

} // namespace piejam
