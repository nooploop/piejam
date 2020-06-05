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

#include <piejam/container/box.h>

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include <functional>
#include <tuple>

namespace piejam::container
{

namespace detail
{

template <class F, class R, class Args>
struct with_boxed_result;

template <class F, class R, class... Args>
struct with_boxed_result<F, R, std::tuple<Args...>>
{
    F f;

    auto operator()(Args... args) const -> box<R>
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

} // namespace piejam::container
