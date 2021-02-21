// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/add_bus.h>

#include <piejam/runtime/state.h>

#include <fmt/format.h>

namespace piejam::runtime::actions
{

auto
add_bus::reduce(state const& st) const -> state
{
    auto new_st = st;

    switch (direction)
    {
        case io_direction::input:
        {
            add_device_bus<io_direction::input>(
                    new_st,
                    fmt::format(
                            "{} In {}",
                            type == audio::bus_type::mono ? "Mono" : "Stereo",
                            new_st.device_io_state.inputs->size() + 1),
                    type,
                    channel_index_pair{npos});
            break;
        }

        case io_direction::output:
        {
            add_device_bus<io_direction::output>(
                    new_st,
                    fmt::format(
                            "Speaker {}",
                            new_st.device_io_state.outputs->size() + 1),
                    audio::bus_type::stereo,
                    channel_index_pair{npos});
            break;
        }
    }

    return new_st;
}

} // namespace piejam::runtime::actions
