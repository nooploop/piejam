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
    boxed_string name;

    io_address_t in;
    io_address_t out;

    float_parameter_id volume;
    float_parameter_id pan_balance;
    bool_parameter_id mute;
    stereo_level_parameter_id level;

    box<fx::chain_t> fx_chain{};

    bool operator==(bus const&) const noexcept = default;
};

struct state
{
    buses_t buses;

    box<bus_list_t> inputs;
    box<bus_list_t> outputs;
    bus_id main;

    bus_id input_solo_id;
    bus_id output_solo_id;
};

template <io_direction D>
auto
bus_ids(state const& st) -> bus_list_t const&
{
    return D == io_direction::input ? st.inputs : st.outputs;
}

template <io_direction D>
auto
bus_ids(state& st) -> box<bus_list_t>&
{
    return D == io_direction::input ? st.inputs : st.outputs;
}

template <io_direction D>
auto
get_bus(state const& st, std::size_t index) -> bus const&
{
    BOOST_ASSERT(index < bus_ids<D>(st).size());
    return *st.buses[bus_ids<D>(st)[index]];
}

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

} // namespace piejam::runtime::mixer
