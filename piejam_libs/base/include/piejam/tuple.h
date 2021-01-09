// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
