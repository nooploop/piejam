// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/add_bus.h>

#include <piejam/runtime/state.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
add_bus::reduce(state const& st) const -> state
{
    auto new_st = st;

    auto bus_name = [this](auto const& st) {
        switch (direction)
        {
            case io_direction::input:
                return fmt::format(
                        "{} In {}",
                        type == audio::bus_type::mono ? "Mono" : "Stereo",
                        st.device_io_state.inputs->size() + 1);

            case io_direction::output:
                return fmt::format(
                        "Speaker {}",
                        st.device_io_state.outputs->size() + 1);
        }
    }(new_st);

    BOOST_ASSERT(
            direction != io_direction::output ||
            type == audio::bus_type::stereo);

    add_device_bus(
            new_st,
            std::move(bus_name),
            direction,
            type,
            channel_index_pair{npos});

    return new_st;
}

} // namespace piejam::runtime::actions
