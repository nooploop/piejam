// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/mp11/algorithm.hpp>

namespace piejam::tuple
{

template <class Tuple>
using decay_elements_t = boost::mp11::mp_transform<std::decay_t, Tuple>;

} // namespace piejam::tuple
