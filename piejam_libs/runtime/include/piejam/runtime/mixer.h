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

#include <functional>
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

using buses_t = entity_map<bus>;
using bus_id = typename buses_t::id_t;
using bus_list_t = std::vector<bus_id>;

struct state
{
    buses_t buses;

    bus_list_t inputs;
    bus_list_t outputs;
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
bus_ids(state& st) -> bus_list_t&
{
    return D == audio::bus_direction::input ? st.inputs : st.outputs;
}

template <audio::bus_direction D>
auto
get_bus(state const& st, std::size_t index) -> bus const&
{
    BOOST_ASSERT(index < bus_ids<D>(st).size());
    return st.buses[bus_ids<D>(st)[index]];
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
    auto const& ids = bus_ids<D>(new_st);
    std::invoke(std::forward<Field>(field), new_st.buses[ids[index]]) =
            std::forward<Value>(value);
    return new_st;
}

inline auto
add_bus(buses_t& buses, bus_list_t& ids, bus&& b) -> bus_id
{
    return ids.emplace_back(buses.add(std::move(b)));
}

template <audio::bus_direction D>
auto
add_bus(state& st, bus&& b) -> bus_id
{
    return add_bus(st.buses, bus_ids<D>(st), std::move(b));
}

inline void
remove_bus(buses_t& buses, bus_list_t& bus_ids, std::size_t index)
{
    buses.remove(bus_ids[index]);
    bus_ids.erase(bus_ids.begin() + index);
}

template <audio::bus_direction D>
void
remove_bus(state& st, std::size_t index)
{
    remove_bus(st.buses, bus_ids<D>(st), index);
}

inline void
clear_buses(buses_t& buses, bus_list_t& bus_ids)
{
    for (auto const& id : bus_ids)
        buses.remove(id);

    bus_ids.clear();
}

template <audio::bus_direction D>
void
clear_buses(state& st)
{
    clear_buses(st.buses, bus_ids<D>(st));
}

} // namespace piejam::runtime::mixer
