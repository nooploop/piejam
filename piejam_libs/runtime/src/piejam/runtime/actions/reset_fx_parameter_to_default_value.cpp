// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/reset_fx_parameter_to_default_value.h>

#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::runtime::actions
{

auto
reset_fx_parameter_to_default_value(fx::parameter_id const param_id)
        -> thunk_action
{
    return [=](auto&& get_state, auto&& dispatch) {
        std::visit(
                [&](auto&& typed_param_id) {
                    state const& st = get_state();
                    if (auto const* const param_desc =
                                st.params.find(typed_param_id);
                        param_desc)
                    {
                        dispatch(set_parameter_value{
                                typed_param_id,
                                param_desc->param.default_value});
                    }
                },
                param_id);
    };
}

} // namespace piejam::runtime::actions
