// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/request_mixer_levels_update.h>

#include <piejam/runtime/actions/request_parameters_update.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <vector>

namespace piejam::runtime::actions
{

auto
request_mixer_levels_update(mixer::channel_ids_t channel_ids) -> thunk_action
{
    return [channel_ids = std::move(
                    channel_ids)](auto const& get_state, auto const& dispatch) {
        state const& st = get_state();
        request_parameters_update next_action;

        for (mixer::channel_id const bus_id : channel_ids)
        {
            next_action.push_back(st.mixer_state.channels[bus_id].level);
        }

        dispatch(next_action);
    };
}

} // namespace piejam::runtime::actions
