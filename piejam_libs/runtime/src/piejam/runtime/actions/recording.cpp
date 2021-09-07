// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/recording.h>

#include <piejam/runtime/actions/request_streams_update.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/runtime/state.h>

#include <boost/assert.hpp>
#include <boost/range/adaptor/map.hpp>

namespace piejam::runtime::actions
{

auto
start_recording::reduce(state const& st) const -> state
{
    BOOST_ASSERT(!st.recording);
    BOOST_ASSERT(st.recorder_streams->empty());

    auto new_st = st;

    new_st.recording = true;

    recorder_streams_t recorder_streams;

    for (auto const& [mixer_channel_id, mixer_channel] :
         new_st.mixer_state.channels)
    {
        if (get_parameter_value(st.params, mixer_channel.record))
        {
            auto stream_id = new_st.streams.add(2);
            recorder_streams.emplace(mixer_channel_id, stream_id);
        }
    }

    new_st.recorder_streams = std::move(recorder_streams);

    return new_st;
}

auto
stop_recording::reduce(state const& st) const -> state
{
    BOOST_ASSERT(st.recording);

    auto new_st = st;

    new_st.recording = false;
    ++new_st.rec_take;

    for (auto const& [mixer_channel_id, stream_id] : *new_st.recorder_streams)
    {
        new_st.streams.remove(stream_id);
    }

    new_st.recorder_streams = {};

    return new_st;
}

auto
request_recorder_streams_update() -> thunk_action
{
    return [](auto const& get_state, auto const& dispatch) {
        state const& st = get_state();
        if (st.recording && !st.recorder_streams->empty())
        {
            auto recorder_streams =
                    *st.recorder_streams | boost::adaptors::map_values;
            request_streams_update action;
            action.streams.insert(
                    recorder_streams.begin(),
                    recorder_streams.end());

            dispatch(action);
        }
    };
}

} // namespace piejam::runtime::actions
