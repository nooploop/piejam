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
#include <variant>
#include <vector>

namespace piejam::runtime::mixer
{

using stereo_level = audio::pair<float>;
using channel_type = audio::bus_type;

struct channel
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

using channel_id = entity_id<struct channel_tag>;

using channel_ids_t = std::vector<channel_id>;
using channels_t = entity_map<channel_id, channel>;

struct state
{
    entity_map<channel_id, channel> channels;

    channel_ids_t inputs;
    channel_ids_t outputs;
    channel_id input_solo_id;
};

inline auto
input_channel(state& st, std::size_t index) -> channel&
{
    BOOST_ASSERT(index < st.inputs.size());
    return st.channels[st.inputs[index]];
}

inline auto
input_channel(state const& st, std::size_t index) -> channel const&
{
    BOOST_ASSERT(index < st.inputs.size());
    return st.channels[st.inputs[index]];
}

inline auto
output_channel(state& st, std::size_t index) -> channel&
{
    BOOST_ASSERT(index < st.outputs.size());
    return st.channels[st.outputs[index]];
}

inline auto
output_channel(state const& st, std::size_t index) -> channel const&
{
    BOOST_ASSERT(index < st.outputs.size());
    return st.channels[st.outputs[index]];
}

inline auto
add_channel(channels_t& chs, channel_ids_t& ch_ids, channel&& ch)
{
    auto id = chs.add(std::move(ch));
    ch_ids.push_back(id);
    return id;
}

inline void
clear_channels(channels_t& chs, channel_ids_t& ch_ids)
{
    for (auto const& id : ch_ids)
        chs.remove(id);

    ch_ids.clear();
}

} // namespace piejam::runtime::mixer
