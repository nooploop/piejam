// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/solo_group.h>

#include <piejam/runtime/mixer.h>

namespace piejam::runtime
{

auto
solo_groups(mixer::buses_t const& mixer_buses) -> solo_groups_t
{
    solo_groups_t result;

    for (auto const& [id, bus] : mixer_buses)
    {
        if (mixer::bus_id const* const target =
                    std::get_if<mixer::bus_id>(&bus.out))
        {
            result[*target].emplace_back(bus.solo, id);
        }
    }

    return result;
}

} // namespace piejam::runtime
