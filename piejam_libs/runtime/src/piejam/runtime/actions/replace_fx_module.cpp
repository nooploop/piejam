// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/replace_fx_module.h>

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::runtime::actions
{

auto
make_replace_fx_module_action(
        state const& st,
        mixer::channel_id const fx_chain_id,
        std::size_t const position,
        ladspa::plugin_id_t const plugin_id,
        std::string_view const name,
        std::vector<fx::parameter_value_assignment> const& initial_values,
        std::vector<fx::parameter_midi_assignment> const& midi_assigns)
        -> batch_action
{
    batch_action batch;

    mixer::channel const& mixer_channel = st.mixer_state.channels[fx_chain_id];

    if (position < mixer_channel.fx_chain->size())
    {
        auto delete_action = std::make_unique<actions::delete_fx_module>();
        delete_action->fx_chain_id = fx_chain_id;
        delete_action->fx_mod_id = (*mixer_channel.fx_chain)[position];
        batch.push_back(std::move(delete_action));
    }

    auto insert_action = std::make_unique<actions::load_ladspa_fx_plugin>();
    insert_action->fx_chain_id = fx_chain_id;
    insert_action->position = position;
    insert_action->plugin_id = plugin_id;
    insert_action->name = name;
    insert_action->initial_values = initial_values;
    insert_action->midi_assignments = midi_assigns;
    batch.push_back(std::move(insert_action));

    return batch;
}

auto
replace_fx_module(
        mixer::channel_id const fx_chain_id,
        std::size_t const position,
        ladspa::plugin_id_t const plugin_id,
        std::string_view const name) -> thunk_action
{
    return [=, name = std::string(name)](auto&& get_state, auto&& dispatch) {
        dispatch(make_replace_fx_module_action(
                get_state(),
                fx_chain_id,
                position,
                plugin_id,
                name,
                {},
                {}));
    };
}

} // namespace piejam::runtime::actions
