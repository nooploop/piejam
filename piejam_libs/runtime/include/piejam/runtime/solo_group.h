// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id_hash.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

#include <unordered_map>
#include <vector>

namespace piejam::runtime
{

struct solo_group_member
{
    bool_parameter_id solo_param;
    mixer::bus_id mixer_bus;
};

using solo_group_t = std::vector<solo_group_member>;
using solo_groups_t = std::unordered_map<mixer::bus_id, solo_group_t>;

auto solo_groups(mixer::buses_t const&) -> solo_groups_t;

} // namespace piejam::runtime
