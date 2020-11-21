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
#include <piejam/container/boxed_string.h>
#include <piejam/entity_map.h>
#include <piejam/npos.h>
#include <piejam/runtime/channel_index_pair.h>
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
    container::boxed_string name;
    float_parameter_id volume;
    float_parameter_id pan_balance;
    bool_parameter_id mute;
    stereo_level_parameter_id level;
    channel_type type{};

    //! mono channels hold same device channel in the pair
    channel_index_pair device_channels{npos};
};

struct state
{
    buses_t buses;

    container::box<bus_list_t> inputs;
    container::box<bus_list_t> outputs;
    bus_id input_solo_id;
};

template <audio::bus_direction D>
auto
bus_ids(state const& st) -> bus_list_t const&
{
    return D == audio::bus_direction::input ? st.inputs : st.outputs;
}

template <audio::bus_direction D>
auto
bus_ids(state& st) -> container::box<bus_list_t>&
{
    return D == audio::bus_direction::input ? st.inputs : st.outputs;
}

template <audio::bus_direction D>
auto
get_bus(state const& st, std::size_t index) -> bus const&
{
    BOOST_ASSERT(index < bus_ids<D>(st).size());
    return *st.buses[bus_ids<D>(st)[index]];
}

template <audio::bus_direction D, class Field, class Value>
[[nodiscard]] auto
update_bus_field(
        state const& st,
        std::size_t index,
        Field&& field,
        Value&& value) -> state
{
    auto new_st = st;
    auto const& ids = bus_ids<D>(new_st).get();
    std::invoke(std::forward<Field>(field), new_st.buses[ids[index]]) =
            std::forward<Value>(value);
    return new_st;
}

} // namespace piejam::runtime::mixer
