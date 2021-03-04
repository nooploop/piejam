// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>
#include <piejam/entity_map.h>
#include <piejam/io_direction.h>
#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/stereo_level.h>

#include <boost/assert.hpp>

#include <functional>
#include <string>
#include <vector>

namespace piejam::runtime::mixer
{

struct bus
{
    boxed_string name{};

    io_address_t in{};
    io_address_t out{};

    float_parameter_id volume;
    float_parameter_id pan_balance;
    bool_parameter_id mute;
    bool_parameter_id solo;
    stereo_level_parameter_id level;

    box<fx::chain_t> fx_chain{};

    bool operator==(bus const&) const noexcept = default;
};

struct state
{
    buses_t buses;

    box<bus_list_t> inputs;
    bus_id main;

    bus_id input_solo_id;
    bus_id output_solo_id;
};

template <io_direction D>
auto
solo_id(state const& st) -> bus_id
{
    return D == io_direction::input ? st.input_solo_id : st.output_solo_id;
}

template <io_direction D>
auto
solo_id(state& st) -> bus_id&
{
    return D == io_direction::input ? st.input_solo_id : st.output_solo_id;
}

bool is_default_source_valid(buses_t const&, bus_id);

auto valid_source_channels(buses_t const&, bus_id)
        -> std::vector<mixer::bus_id>;

auto valid_target_channels(buses_t const&, bus_id)
        -> std::vector<mixer::bus_id>;

} // namespace piejam::runtime::mixer
