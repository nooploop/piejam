// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
