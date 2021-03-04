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

#include <spdlog/spdlog.h>

namespace piejam::runtime::actions
{

auto
make_replace_fx_module_action(
        state const& st,
        mixer::channel_id const fx_chain_bus,
        std::size_t const position,
        fx::internal const fx_type) -> batch_action
{
    batch_action batch;

    mixer::channel const& bus = st.mixer_state.channels[fx_chain_bus];

    if (position < bus.fx_chain->size())
    {
        auto delete_action = std::make_unique<actions::delete_fx_module>();
        delete_action->fx_mod_id = (*bus.fx_chain)[position];
        batch.push_back(std::move(delete_action));
    }

    auto insert_action = std::make_unique<actions::insert_internal_fx_module>();
    insert_action->fx_chain_bus = fx_chain_bus;
    insert_action->position = position;
    insert_action->type = fx_type;
    batch.push_back(std::move(insert_action));

    return batch;
}

auto
replace_fx_module(
        mixer::channel_id const fx_chain_channel,
        std::size_t const position,
        fx::internal const fx_type) -> thunk_action
{
    return [=](auto&& get_state, auto&& dispatch) {
        dispatch(make_replace_fx_module_action(
                get_state(),
                fx_chain_channel,
                position,
                fx_type));
    };
}

auto
replace_fx_module(std::size_t const position, fx::internal const fx_type)
        -> thunk_action
{
    return [=](auto&& get_state, auto&& dispatch) {
        dispatch(make_replace_fx_module_action(
                get_state(),
                get_state().fx_chain_channel,
                position,
                fx_type));
    };
}

auto
make_replace_fx_module_action(
        state const& st,
        mixer::channel_id const fx_chain_channel,
        std::size_t const position,
        audio::ladspa::plugin_id_t const plugin_id,
        std::string_view const& name,
        std::vector<fx::parameter_value_assignment> const& initial_values)
        -> batch_action
{
    batch_action batch;

    mixer::channel const& bus = st.mixer_state.channels[fx_chain_channel];

    if (position < bus.fx_chain->size())
    {
        auto delete_action = std::make_unique<actions::delete_fx_module>();
        delete_action->fx_mod_id = (*bus.fx_chain)[position];
        batch.push_back(std::move(delete_action));
    }

    auto insert_action = std::make_unique<actions::load_ladspa_fx_plugin>();
    insert_action->fx_chain_bus = fx_chain_channel;
    insert_action->position = position;
    insert_action->plugin_id = plugin_id;
    insert_action->name = name;
    insert_action->initial_values = initial_values;
    batch.push_back(std::move(insert_action));

    return batch;
}

auto
replace_fx_module(
        mixer::channel_id const fx_chain_channel,
        std::size_t const position,
        audio::ladspa::plugin_id_t const plugin_id,
        std::string_view const& name) -> thunk_action
{
    return [=, name = std::string(name)](auto&& get_state, auto&& dispatch) {
        dispatch(make_replace_fx_module_action(
                get_state(),
                fx_chain_channel,
                position,
                plugin_id,
                name,
                {}));
    };
}

auto
replace_fx_module(
        std::size_t const position,
        audio::ladspa::plugin_id_t const plugin_id,
        std::string_view const& name) -> thunk_action
{
    return [=, name = std::string(name)](auto&& get_state, auto&& dispatch) {
        dispatch(make_replace_fx_module_action(
                get_state(),
                get_state().fx_chain_channel,
                position,
                plugin_id,
                name,
                {}));
    };
}

} // namespace piejam::runtime::actions
