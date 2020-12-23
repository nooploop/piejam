// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/audio/types.h>
#include <piejam/boxed_string.h>
#include <piejam/entity_map.h>
#include <piejam/io_direction.h>
#include <piejam/npos.h>
#include <piejam/runtime/channel_index_pair.h>
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

using channel_type = audio::bus_type;

struct bus
{
    boxed_string name;
    float_parameter_id volume;
    float_parameter_id pan_balance;
    bool_parameter_id mute;
    stereo_level_parameter_id level;
    channel_type type{};

    //! mono channels hold same device channel in the pair
    channel_index_pair device_channels{npos};

    box<fx::chain_t> fx_chain{};
};

struct state
{
    buses_t buses;

    box<bus_list_t> inputs;
    box<bus_list_t> outputs;
    bus_id input_solo_id;
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

} // namespace piejam::runtime::mixer
