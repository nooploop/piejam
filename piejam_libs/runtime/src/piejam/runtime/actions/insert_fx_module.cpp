// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/insert_fx_module.h>

#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/ladspa_fx/ladspa_fx_module.h>
#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

void
insert_internal_fx_module::reduce(state& st) const
{
    auto fx_mod_id = runtime::insert_internal_fx_module(
            st,
            fx_chain_id,
            position,
            type,
            initial_values,
            midi_assignments);

    if (show_fx_module)
    {
        st.gui_state.focused_fx_chain_id = fx_chain_id;
        st.gui_state.focused_fx_mod_id = fx_mod_id;
        st.gui_state.root_view_mode_ = runtime::root_view_mode::fx_module;
    }
}

void
insert_ladspa_fx_module::reduce(state& st) const
{
    auto fx_mod_id = runtime::insert_ladspa_fx_module(
            st,
            fx_chain_id,
            position,
            instance_id,
            plugin_desc,
            control_inputs,
            initial_values,
            midi_assignments);

    if (show_fx_module)
    {
        st.gui_state.focused_fx_chain_id = fx_chain_id;
        st.gui_state.focused_fx_mod_id = fx_mod_id;
        st.gui_state.root_view_mode_ = runtime::root_view_mode::fx_module;
    }
}

void
insert_missing_ladspa_fx_module::reduce(state& st) const
{
    runtime::insert_missing_ladspa_fx_module(
            st,
            fx_chain_id,
            position,
            unavailable_ladspa,
            name);
}

void
replace_missing_ladspa_fx_module::reduce(state& st) const
{
    auto mixer_channels = st.mixer_state.channels.lock();
    auto fx_modules = st.fx_modules.lock();
    auto unavail_ladspas = st.fx_unavailable_ladspa_plugins.lock();

    for (auto const& [fx_chain_id, replacements] : fx_chain_replacements)
    {
        auto& mixer_channel = mixer_channels[fx_chain_id];
        auto fx_chain = mixer_channel.fx_chain.lock();

        for (auto const& [pos, ladspa_instance] : replacements)
        {
            BOOST_ASSERT(pos < fx_chain->size());
            auto const prev_fx_mod_id = (*fx_chain)[pos];

            auto unavail_id = std::get<fx::unavailable_ladspa_id>(
                    fx_modules[prev_fx_mod_id].fx_instance_id);

            fx_modules.erase(prev_fx_mod_id);

            auto const& unavail = unavail_ladspas[unavail_id];

            BOOST_ASSERT(unavail.plugin_id == ladspa_instance.plugin_desc.id);
            BOOST_ASSERT(
                    audio::num_channels(mixer_channel.bus_type) ==
                    ladspa_instance.plugin_desc.num_inputs);
            BOOST_ASSERT(
                    audio::num_channels(mixer_channel.bus_type) ==
                    ladspa_instance.plugin_desc.num_outputs);

            auto const fx_mod_id = fx_modules.emplace(ladspa_fx::make_module(
                    ladspa_instance.instance_id,
                    ladspa_instance.plugin_desc.name,
                    mixer_channel.bus_type,
                    ladspa_instance.control_inputs,
                    st.params));

            (*fx_chain)[pos] = fx_mod_id;

            auto const& fx_mod = fx_modules[fx_mod_id];
            apply_parameter_values(unavail.parameter_values, fx_mod, st.params);
            apply_fx_midi_assignments(
                    unavail.midi_assignments,
                    fx_mod,
                    *st.midi_assignments.lock());

            unavail_ladspas.erase(unavail_id);

            st.fx_ladspa_instances.emplace(
                    ladspa_instance.instance_id,
                    ladspa_instance.plugin_desc);
        }
    }
}

} // namespace piejam::runtime::actions
