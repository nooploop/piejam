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
            auto name =
                    fmt::format("In {}", new_st.mixer_state.inputs->size() + 1);
            runtime::add_mixer_bus<io_direction::input>(
                    new_st,
                    name,
                    add_device_bus<io_direction::input>(
                            new_st,
                            name,
                            type,
                            channel_index_pair{npos}));
            break;
        }

        case io_direction::output:
        {
            auto const bus_ids_size = new_st.mixer_state.outputs->size();
            auto name = bus_ids_size == 0 ? std::string("Main")
                                          : fmt::format("Aux {}", bus_ids_size);
            runtime::add_mixer_bus<io_direction::output>(
                    new_st,
                    name,
                    add_device_bus<io_direction::output>(
                            new_st,
                            name,
                            audio::bus_type::stereo,
                            channel_index_pair{}));
            break;
        }
    }

    return new_st;
}

} // namespace piejam::runtime::actions
