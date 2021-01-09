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
            add_mixer_bus<io_direction::input>(
                    new_st,
                    fmt::format("In {}", new_st.mixer_state.inputs->size() + 1),
                    type);
            break;

        case io_direction::output:
        {
            auto const bus_ids_size = new_st.mixer_state.outputs->size();
            add_mixer_bus<io_direction::output>(
                    new_st,
                    (bus_ids_size == 0 ? std::string("Main")
                                       : fmt::format("Aux {}", bus_ids_size)),
                    type);
            break;
        }
    }

    return new_st;
}

} // namespace piejam::runtime::actions
