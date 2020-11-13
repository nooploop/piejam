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

#include <tuple>

namespace piejam::tuple
{

template <template <class> class F, class Tuple>
struct meta_apply;

template <template <class> class F, class... Args>
struct meta_apply<F, std::tuple<Args...>>
{
    using type = std::tuple<F<Args>...>;
};

template <template <class> class F, class First, class Second>
struct meta_apply<F, std::pair<First, Second>>
{
    using type = std::pair<F<First>, F<Second>>;
};

template <class Tuple>
using decay_elements_t = typename meta_apply<std::decay_t, Tuple>::type;

} // namespace piejam::tuple
