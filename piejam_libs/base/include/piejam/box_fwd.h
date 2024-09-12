// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam
{

namespace detail
{

struct box_equal;
struct box_eq;

template <class T, class Eq>
class box;

} // namespace detail

template <class T>
using box = detail::box<T, detail::box_equal>;

template <class T>
using unique_box = detail::box<T, detail::box_eq>;

} // namespace piejam
