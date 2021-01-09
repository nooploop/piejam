// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>

#include <vector>

namespace piejam
{

template <class T>
using boxed_vector = box<std::vector<T>>;

} // namespace piejam
