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
request_mixer_levels_update(std::vector<mixer::bus_id> bus_ids) -> thunk_action
{
    return [bus_ids = std::move(
                    bus_ids)](auto const& get_state, auto const& dispatch) {
        state const& st = get_state();
        request_parameters_update next_action;

        for (mixer::bus_id const bus_id : bus_ids)
        {
            next_action.push_back(st.mixer_state.buses[bus_id].level);
        }

        dispatch(next_action);
    };
}

} // namespace piejam::runtime::actions
