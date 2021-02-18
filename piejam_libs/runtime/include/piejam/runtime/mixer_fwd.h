// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/fwd.h>

#include <vector>

namespace piejam::runtime::mixer
{

struct bus;
using bus_id = entity_id<bus>;
using buses_t = entity_map<bus>;

using bus_list_t = std::vector<bus_id>;

struct state;

} // namespace piejam::runtime::mixer
