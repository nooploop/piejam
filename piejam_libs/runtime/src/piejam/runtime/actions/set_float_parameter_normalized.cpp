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

#include <piejam/runtime/actions/set_float_parameter_normalized.h>

#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::runtime::actions
{

auto
set_float_parameter_normalized(float_parameter_id param_id, float norm_value)
        -> thunk_action
{
    return [=](auto const& get_state, auto const& dispatch) {
        state const& st = get_state();
        if (float_parameter const* const param =
                    st.params.get_parameter(param_id))
            dispatch(set_float_parameter{
                    param_id,
                    param->from_normalized(*param, norm_value)});
    };
}

} // namespace piejam::runtime::actions
