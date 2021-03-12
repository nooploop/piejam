// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/reload_missing_plugins.h>

#include <piejam/range/indices.h>
#include <piejam/runtime/actions/replace_fx_module.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
reload_missing_plugins() -> thunk_action
{
    return [](auto&& get_state, auto&& dispatch) {
        batch_action batch;

        state const& st = get_state();

        for (auto&& [bus_id, bus] : st.mixer_state.channels)
        {
            for (std::size_t fx_pos : range::indices(*bus.fx_chain))
            {
                auto&& fx_mod_id = (*bus.fx_chain)[fx_pos];
                auto const& fx_mod = st.fx_modules[fx_mod_id];

                if (auto id = std::get_if<fx::unavailable_ladspa_id>(
                            &fx_mod.fx_instance_id))
                {
                    auto const& unavail = st.fx_unavailable_ladspa_plugins[*id];
                    batch.append(actions::make_replace_fx_module_action(
                            st,
                            bus_id,
                            fx_pos,
                            unavail.plugin_id,
                            *fx_mod.name,
                            unavail.parameter_values,
                            unavail.midi_assigns));
                }
            }
        }

        dispatch(batch);
    };
}

} // namespace piejam::runtime::actions
