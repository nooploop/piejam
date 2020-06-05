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

#include <boost/callable_traits/args.hpp>

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

    template <
            class F,
            std::enable_if_t<
                    !std::is_same_v<
                            std::remove_const_t<F>,
                            selector<Value, State>>,
                    bool> = true>
    selector(F&& f)
        : m_f(std::forward<F>(f))
    {
    }

    auto operator()(State const& st) const -> Value { return m_f(st); }

private:
    function_type m_f;
};

} // namespace piejam::reselect
