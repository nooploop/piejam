// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

        for (auto&& [bus_id, bus] : st.mixer_state.buses)
        {
            for (std::size_t fx_pos : range::indices(bus.fx_chain->size()))
            {
                auto&& fx_mod_id = (*bus.fx_chain)[fx_pos];
                auto fx_mod = st.fx_modules[fx_mod_id];
                BOOST_ASSERT(fx_mod);

                if (auto id = std::get_if<fx::unavailable_ladspa_id>(
                            &fx_mod->fx_instance_id))
                {
                    auto unavail = st.fx_unavailable_ladspa_plugins[*id];
                    BOOST_ASSERT(unavail);
                    batch.append(actions::make_replace_fx_module_action(
                            st,
                            bus_id,
                            fx_pos,
                            unavail->plugin_id,
                            *fx_mod->name,
                            unavail->parameter_assignments));
                }
            }
        }

        dispatch(batch);
    };
}

} // namespace piejam::runtime::actions
