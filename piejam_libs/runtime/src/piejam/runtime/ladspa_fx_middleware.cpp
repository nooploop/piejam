// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/ladspa_fx_middleware.h>

#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/ladspa_fx_action.h>
#include <piejam/runtime/fx/ladspa_control.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/action.h>

#include <spdlog/spdlog.h>

namespace piejam::runtime
{

ladspa_fx_middleware::ladspa_fx_middleware(
        middleware_functors mw_fs,
        fx::ladspa_control& ladspa_control)
    : middleware_functors(std::move(mw_fs))
    , m_ladspa_control(ladspa_control)
{
}

void
ladspa_fx_middleware::operator()(action const& action)
{
    if (auto const* const a =
                dynamic_cast<actions::ladspa_fx_action const*>(&action))
    {
        auto v = ui::make_action_visitor<actions::ladspa_fx_action_visitor>(
                [this](auto const& a) { process_ladspa_fx_action(a); });

        a->visit(v);
    }
    else
    {
        next(action);
    }
}

template <>
void
ladspa_fx_middleware::process_ladspa_fx_action(
        actions::load_ladspa_fx_plugin const& a)
{
    auto const& st = get_state();

    if (auto plugin_desc =
                find_ladspa_plugin_descriptor(st.fx_registry, a.plugin_id))
    {
        if (auto id = m_ladspa_control.load(*plugin_desc))
        {
            actions::insert_ladspa_fx_module next_action;
            next_action.fx_chain_bus = a.fx_chain_bus;
            next_action.position = a.position;
            next_action.instance_id = id;
            next_action.plugin_desc = *plugin_desc;
            next_action.control_inputs = m_ladspa_control.control_inputs(id);
            next_action.initial_values = a.initial_values;
            next_action.midi_assigns = a.midi_assigns;

            next(next_action);

            return;
        }
        else
        {
            spdlog::error("failed to load ladspa fx plugin: {}", a.name);
        }
    }

    actions::insert_missing_ladspa_fx_module next_action;
    next_action.fx_chain_bus = a.fx_chain_bus;
    next_action.position = a.position;
    next_action.unavailable_ladspa = fx::unavailable_ladspa{
            .plugin_id = a.plugin_id,
            .parameter_values = a.initial_values,
            .midi_assigns = a.midi_assigns};
    next_action.name = a.name;
    next(next_action);
}

template <>
void
ladspa_fx_middleware::process_ladspa_fx_action(
        actions::delete_fx_module const& a)
{
    auto const& st = get_state();

    if (fx::module const* const fx_mod = st.fx_modules[a.fx_mod_id])
    {
        auto const fx_instance_id = fx_mod->fx_instance_id;

        next(a);

        if (fx::ladspa_instance_id const* const instance_id =
                    std::get_if<fx::ladspa_instance_id>(&fx_instance_id))
        {
            m_ladspa_control.unload(*instance_id);
        }
    }
}

} // namespace piejam::runtime
