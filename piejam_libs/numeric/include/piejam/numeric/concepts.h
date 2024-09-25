// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/numeric/type_traits.h>

namespace piejam::numeric
{

template <class T>
concept integral = is_integral_v<T>;

} // namespace piejam::numeric
