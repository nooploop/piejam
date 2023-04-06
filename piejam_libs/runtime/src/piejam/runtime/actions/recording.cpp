// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/recording.h>

#include <piejam/runtime/actions/request_streams_update.h>
#include <piejam/runtime/state.h>

#include <ranges>

namespace piejam::runtime::actions
{

auto
start_recording::reduce(state const& st) const -> state
{
    BOOST_ASSERT_MSG(false, "not reducible");
    return st;
}

auto
stop_recording::reduce(state const& st) const -> state
{
    BOOST_ASSERT_MSG(false, "not reducible");
    return st;
}

auto
request_recorder_streams_update() -> thunk_action
{
    return [](auto const& get_state, auto const& dispatch) {
        state const& st = get_state();
        if (st.recording && !st.recorder_streams->empty())
        {
            auto recorder_streams = *st.recorder_streams | std::views::values;
            request_streams_update action;
            action.streams.insert(
                    recorder_streams.begin(),
                    recorder_streams.end());

            dispatch(action);
        }
    };
}

} // namespace piejam::runtime::actions
