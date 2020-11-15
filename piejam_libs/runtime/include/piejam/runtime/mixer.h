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

#include <boost/assert.hpp>

#include <string>
#include <vector>

namespace piejam::runtime::mixer
{

using stereo_level = audio::pair<float>;
using channel_type = audio::bus_type;

struct bus
{
    container::boxed_string name;
    float volume{1.f};
    float pan_balance{};
    bool mute{};
    stereo_level level{};
    channel_type type{};

    //! mono channels hold same device channel in the pair
    channel_index_pair device_channels{npos};
};

using bus_id = entity_id<struct bus_id_tag>;

using bus_ids_t = std::vector<bus_id>;
using buses_t = entity_map<bus_id, bus>;

struct state
{
    entity_map<bus_id, bus> buses;

    bus_ids_t inputs;
    bus_ids_t outputs;
    bus_id input_solo_id;
};

inline auto
input_bus(state& st, std::size_t index) -> bus&
{
    BOOST_ASSERT(index < st.inputs.size());
    return st.buses[st.inputs[index]];
}

inline auto
input_bus(state const& st, std::size_t index) -> bus const&
{
    BOOST_ASSERT(index < st.inputs.size());
    return st.buses[st.inputs[index]];
}

inline auto
output_bus(state& st, std::size_t index) -> bus&
{
    BOOST_ASSERT(index < st.outputs.size());
    return st.buses[st.outputs[index]];
}

inline auto
output_bus(state const& st, std::size_t index) -> bus const&
{
    BOOST_ASSERT(index < st.outputs.size());
    return st.buses[st.outputs[index]];
}

inline auto
add_bus(buses_t& buses, bus_ids_t& bus_ids, bus&& b)
{
    auto id = buses.add(std::move(b));
    bus_ids.push_back(id);
    return id;
}

inline void
clear_buses(buses_t& buses, bus_ids_t& bus_ids)
{
    for (auto const& id : bus_ids)
        buses.remove(id);

    bus_ids.clear();
}

} // namespace piejam::runtime::mixer
