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

#include <piejam/runtime/actions/request_mixer_levels_update.h>

#include <piejam/runtime/actions/request_levels_update.h>
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
        request_levels_update next_action;
        for (mixer::bus_id const bus_id : bus_ids)
        {
            if (mixer::bus const* const bus = st.mixer_state.buses[bus_id])
                next_action.level_ids.emplace_back(bus->level);
        }

        dispatch(next_action);
    };
}

} // namespace piejam::runtime::actions
