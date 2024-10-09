// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/reset_fx_parameter_to_default_value.h>

#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::runtime::actions
{

auto
reset_fx_parameter_to_default_value(parameter_id param_id) -> thunk_action
{
    return [=](auto&& get_state, auto&& dispatch) {
        std::visit(
                [&]<class P>(parameter::id_t<P> typed_param_id) {
                    if constexpr (parameter::has_default_value<P>)
                    {
                        state const& st = get_state();
                        if (auto param_desc = st.params.find(typed_param_id);
                            param_desc)
                        {
                            dispatch(set_parameter_value{
                                    typed_param_id,
                                    param_desc->param.default_value});
                        }
                    }
                },
                param_id);
    };
}

} // namespace piejam::runtime::actions
