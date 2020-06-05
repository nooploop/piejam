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

#include <piejam/meta/decay_tuple_elements.h>

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace piejam
{

template <class F>
class memoized
{
public:
    memoized(F const& f) noexcept(std::is_nothrow_copy_constructible_v<F>)
        : m_f(f)
    {
    }
    memoized(F&& f) noexcept(std::is_nothrow_move_constructible_v<F>)
        : m_f(std::move(f))
    {
    }

    template <class... Args>
    auto operator()(Args&&... args) -> std::add_lvalue_reference_t<
            std::add_const_t<std::invoke_result_t<F, Args&&...>>>
    {
        if (!m_last ||
            m_last->args != std::tuple<Args&&...>(std::forward<Args>(args)...))
        {
            m_last.emplace(
                    std::tuple<Args...>(args...),
                    m_f(std::forward<Args>(args)...));
        }

        return m_last->result;
    }

private:
    struct args_and_result
    {
        using args_tuple_type =
                meta::decay_tuple_elements_t<boost::callable_traits::args_t<F>>;
        using result_type =
                std::decay_t<boost::callable_traits::return_type_t<F>>;

        args_and_result(args_tuple_type args, result_type result)
            : args(std::move(args))
            , result(std::move(result))
        {
        }

        args_tuple_type args;
        result_type result;
    };

    F m_f;

    std::optional<args_and_result> m_last;
};

template <class F>
memoized(F const&) -> memoized<F>;
template <class F>
memoized(F &&) -> memoized<F>;

} // namespace piejam
