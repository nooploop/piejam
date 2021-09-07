// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/solo_group.h>

#include <piejam/runtime/mixer.h>

namespace piejam::runtime
{

auto
solo_groups(mixer::channels_t const& channels) -> solo_groups_t
{
    solo_groups_t result;

    for (auto const& [mixer_channel_id, mixer_channel] : channels)
    {
        if (mixer::channel_id const* const target =
                    std::get_if<mixer::channel_id>(&mixer_channel.out))
        {
            result[*target].emplace_back(mixer_channel.solo, mixer_channel_id);
        }
    }

    return result;
}

} // namespace piejam::runtime
