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

#include <piejam/runtime/session_middleware.h>

#include <piejam/functional/overload.h>
#include <piejam/runtime/actions/apply_session.h>
#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/config_access.h>
#include <piejam/runtime/session.h>

#include <boost/assert.hpp>

namespace piejam::runtime
{

session_middleware::session_middleware(
        locations const& locs,
        get_state_f get_state,
        dispatch_f dispatch,
        next_f next)
    : m_get_state(std::move(get_state))
    , m_dispatch(std::move(dispatch))
    , m_next(std::move(next))
    , m_locations(locs)
{
    BOOST_ASSERT(m_get_state);
    BOOST_ASSERT(m_next);
    BOOST_ASSERT(!m_locations.config_dir.empty());
}

static auto
export_fx_chains(
        audio_state const& st,
        mixer::bus_list_t const& bus_ids,
        fx::instance_plugin_id_map const& plugin_ids)
        -> std::vector<session::fx_chain_data>
{
    std::vector<session::fx_chain_data> result;

    for (auto const& bus_id : bus_ids)
    {
        mixer::bus const* const bus = (*st.mixer_state.buses)[bus_id];

        auto& fx_chain_data = result.emplace_back();

        for (auto const& fx_mod_id : *bus->fx_chain)
        {
            fx::module const* const fx_mod = (*st.fx_modules)[fx_mod_id];
            fx_chain_data.emplace_back(std::visit(
                    overload{
                            [](fx::internal id) -> fx_plugin_id { return id; },
                            [&plugin_ids](
                                    fx::ladspa_instance_id id) -> fx_plugin_id {
                                return plugin_ids.at(id);
                            }},
                    fx_mod->fx_instance_id));
        }
    }

    return result;
}

void
session_middleware::operator()(action const& a)
{
    if (dynamic_cast<actions::save_app_config const*>(&a))
    {
        config_access::save(m_locations, m_get_state());
    }
    else if (dynamic_cast<actions::load_app_config const*>(&a))
    {
        config_access::load(m_locations, m_dispatch);
    }
    else if (auto action = dynamic_cast<actions::save_session const*>(&a))
    {
        auto const& st = m_get_state();

        session ses;

        ses.inputs = export_fx_chains(
                st,
                *st.mixer_state.inputs,
                action->plugin_ids);
        ses.outputs = export_fx_chains(
                st,
                *st.mixer_state.outputs,
                action->plugin_ids);

        save_session(ses, action->file);
    }
    else if (auto action = dynamic_cast<actions::load_session const*>(&a))
    {
        actions::apply_session next_action;
        next_action.ses = load_session(action->file);
        m_dispatch(next_action);
    }
    else
    {
        m_next(a);
    }
}

} // namespace piejam::runtime
