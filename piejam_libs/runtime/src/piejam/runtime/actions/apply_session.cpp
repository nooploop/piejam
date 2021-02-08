// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/apply_session.h>

#include <piejam/range/iota.h>
#include <piejam/runtime/actions/control_midi_assignment.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/persistence/session.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <functional>

namespace piejam::runtime::actions
{

namespace
{

struct make_add_fx_module_action
{
    mixer::bus_id fx_chain_bus;

    auto operator()(persistence::session::internal_fx const& fx) const
            -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::insert_internal_fx_module>();
        action->fx_chain_bus = fx_chain_bus;
        action->position = npos;
        action->type = fx.type;
        action->initial_values = fx.preset;
        action->midi_assigns = fx.midi;
        return action;
    }

    auto operator()(persistence::session::ladspa_plugin const& ladspa_plug)
            -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::load_ladspa_fx_plugin>();
        action->fx_chain_bus = fx_chain_bus;
        action->position = npos;
        action->plugin_id = ladspa_plug.id;
        action->name = ladspa_plug.name;
        action->initial_values = ladspa_plug.preset;
        action->midi_assigns = ladspa_plug.midi;
        return action;
    }
};

void
make_add_fx_module_actions(
        batch_action& batch,
        mixer::bus_list_t const& bus_ids,
        std::vector<persistence::session::mixer_bus> const& mb_data)
{
    for (std::size_t i : range::iota(std::min(mb_data.size(), bus_ids.size())))
    {
        mixer::bus_id fx_chain_bus = bus_ids[i];
        for (auto const& fx_plug : mb_data[i].fx_chain)
        {
            batch.push_back(std::visit(
                    make_add_fx_module_action{fx_chain_bus},
                    fx_plug.as_variant()));
        }
    }
}

void
apply_mixer_midi(
        batch_action& batch,
        mixer::bus_list_t const& bus_ids,
        mixer::buses_t const& buses,
        std::vector<persistence::session::mixer_bus> const& mb_data)
{
    auto action = std::make_unique<actions::update_midi_assignments>();

    for (std::size_t i : range::iota(std::min(mb_data.size(), bus_ids.size())))
    {
        if (mixer::bus const* const bus = buses[bus_ids[i]])
        {
            auto const& mxr_midi = mb_data[i].midi;
            if (mxr_midi.volume)
                action->assignments.emplace(bus->volume, *mxr_midi.volume);

            if (mxr_midi.pan)
                action->assignments.emplace(bus->pan_balance, *mxr_midi.pan);

            if (mxr_midi.mute)
                action->assignments.emplace(bus->mute, *mxr_midi.mute);
        }
    }

    if (!action->assignments.empty())
        batch.push_back(std::move(action));
}

template <class GetState>
auto
configure_mixer_buses(persistence::session const& session, GetState&& get_state)
        -> batch_action
{
    batch_action action;

    state const& st = get_state();

    make_add_fx_module_actions(action, st.mixer_state.inputs, session.inputs);
    make_add_fx_module_actions(action, st.mixer_state.outputs, session.outputs);
    apply_mixer_midi(
            action,
            st.mixer_state.inputs,
            st.mixer_state.buses,
            session.inputs);
    apply_mixer_midi(
            action,
            st.mixer_state.outputs,
            st.mixer_state.buses,
            session.outputs);

    return action;
}

} // namespace

auto
apply_session(persistence::session session) -> thunk_action
{
    return [s = std::move(session)](auto&& get_state, auto&& dispatch) {
        std::invoke(
                std::forward<decltype(dispatch)>(dispatch),
                configure_mixer_buses(
                        s,
                        std::forward<decltype(get_state)>(get_state)));
    };
}

} // namespace piejam::runtime::actions
