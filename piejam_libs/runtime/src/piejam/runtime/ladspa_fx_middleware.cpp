// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/ladspa_fx_middleware.h>

#include <piejam/ladspa/instance_manager.h>
#include <piejam/range/indices.h>
#include <piejam/redux/middleware_functors.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/ladspa_fx_action.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/action.h>

#include <spdlog/spdlog.h>

namespace piejam::runtime
{

ladspa_fx_middleware::ladspa_fx_middleware(
        ladspa::instance_manager& ladspa_control)
    : m_ladspa_control(ladspa_control)
{
}

void
ladspa_fx_middleware::operator()(
        middleware_functors const& mw_fs,
        action const& action)
{
    if (auto const* const a =
                dynamic_cast<actions::ladspa_fx_action const*>(&action))
    {
        auto v = ui::make_action_visitor<actions::ladspa_fx_action_visitor>(
                [&](auto const& a) { process_ladspa_fx_action(mw_fs, a); });

        a->visit(v);
    }
    else
    {
        mw_fs.next(action);
    }
}

template <>
void
ladspa_fx_middleware::process_ladspa_fx_action(
        middleware_functors const& mw_fs,
        actions::load_ladspa_fx_plugin const& a)
{
    if (auto plugin_desc = find_ladspa_plugin_descriptor(
                mw_fs.get_state().fx_registry,
                a.plugin_id))
    {
        if (auto id = m_ladspa_control.load(*plugin_desc); id.valid())
        {
            actions::insert_ladspa_fx_module next_action;
            next_action.fx_chain_id = a.fx_chain_id;
            next_action.position = a.position;
            next_action.instance_id = id;
            next_action.plugin_desc = *plugin_desc;
            next_action.control_inputs = m_ladspa_control.control_inputs(id);
            next_action.initial_values = a.initial_values;
            next_action.midi_assignments = a.midi_assignments;
            next_action.show_fx_module = a.show_fx_module;

            mw_fs.next(next_action);

            return;
        }
        else
        {
            spdlog::error("Failed to load LADSPA fx plugin: {}", a.name);
        }
    }

    actions::insert_missing_ladspa_fx_module next_action;
    next_action.fx_chain_id = a.fx_chain_id;
    next_action.position = a.position;
    next_action.unavailable_ladspa = fx::unavailable_ladspa{
            .plugin_id = a.plugin_id,
            .parameter_values = a.initial_values,
            .midi_assignments = a.midi_assignments};
    next_action.name = a.name;
    mw_fs.next(next_action);
}

template <>
void
ladspa_fx_middleware::process_ladspa_fx_action(
        middleware_functors const& mw_fs,
        actions::delete_fx_module const& a)
{
    fx::module const& fx_mod = mw_fs.get_state().fx_modules[a.fx_mod_id];

    auto const fx_instance_id = fx_mod.fx_instance_id;

    mw_fs.next(a);

    if (ladspa::instance_id const* const instance_id =
                std::get_if<ladspa::instance_id>(&fx_instance_id))
    {
        m_ladspa_control.unload(*instance_id);
    }
}

template <>
void
ladspa_fx_middleware::process_ladspa_fx_action(
        middleware_functors const& mw_fs,
        actions::delete_mixer_channel const& a)
{
    auto const& st = mw_fs.get_state();

    std::vector<fx::instance_id> fx_instance_ids;

    for (auto fx_mod_id :
         st.mixer_state.channels[a.mixer_channel_id].fx_chain.get())
    {
        fx::module const& fx_mod = mw_fs.get_state().fx_modules[fx_mod_id];
        fx_instance_ids.emplace_back(fx_mod.fx_instance_id);
    }

    mw_fs.next(a);

    for (auto fx_instance_id : fx_instance_ids)
    {
        if (ladspa::instance_id const* const instance_id =
                    std::get_if<ladspa::instance_id>(&fx_instance_id))
        {
            m_ladspa_control.unload(*instance_id);
        }
    }
}

template <>
void
ladspa_fx_middleware::process_ladspa_fx_action(
        middleware_functors const& mw_fs,
        actions::reload_missing_plugins const&)
{
    auto const& st = mw_fs.get_state();

    actions::replace_missing_ladspa_fx_module action;

    bool send_action = false;
    for (auto&& [mixer_channel_id, mixer_channel] : st.mixer_state.channels)
    {
        auto& [_, fx_mod_replacements] =
                action.fx_chain_replacements.emplace_back(mixer_channel_id);

        for (std::size_t fx_pos : range::indices(*mixer_channel.fx_chain))
        {
            auto&& fx_mod_id = (*mixer_channel.fx_chain)[fx_pos];
            auto const& fx_mod = st.fx_modules[fx_mod_id];

            if (auto unavail_id = std::get_if<fx::unavailable_ladspa_id>(
                        &fx_mod.fx_instance_id);
                unavail_id)
            {
                auto const& unavail =
                        st.fx_unavailable_ladspa_plugins[*unavail_id];

                if (auto plugin_desc = find_ladspa_plugin_descriptor(
                            st.fx_registry,
                            unavail.plugin_id);
                    plugin_desc)
                {
                    auto const num_channels =
                            audio::num_channels(mixer_channel.bus_type);
                    if (num_channels != plugin_desc->num_inputs ||
                        num_channels != plugin_desc->num_outputs)
                    {
                        continue;
                    }

                    if (auto instance_id = m_ladspa_control.load(*plugin_desc);
                        instance_id.valid())
                    {
                        auto& [_, ladspa_instance] =
                                fx_mod_replacements.emplace_back(fx_pos);
                        ladspa_instance.instance_id = instance_id;
                        ladspa_instance.plugin_desc = *plugin_desc;
                        ladspa_instance.control_inputs =
                                m_ladspa_control.control_inputs(instance_id);

                        send_action = true;
                    }
                    else
                    {
                        spdlog::error(
                                "Failed to load LADSPA fx plugin: {}",
                                *fx_mod.name);
                    }
                }
            }
        }
    }

    if (send_action)
    {
        mw_fs.next(action);
    }
}

} // namespace piejam::runtime
