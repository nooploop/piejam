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

#include <piejam/runtime/actions/apply_session.h>

#include <piejam/range/indices.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/load_ladspa_fx_plugin.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/persistence/session.h>
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

    auto operator()(fx::internal type) const -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::insert_internal_fx_module>();
        action->fx_chain_bus = fx_chain_bus;
        action->position = npos;
        action->type = type;
        return action;
    }

    auto operator()(persistence::session::ladspa_plugin const& ladspa_plug)
            -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::load_ladspa_fx_plugin>();
        action->fx_chain_bus = fx_chain_bus;
        action->plugin_id = ladspa_plug.id;
        action->name = ladspa_plug.name;
        return action;
    }
};

void
make_add_fx_module_actions(
        batch_action& batch,
        mixer::bus_list_t const& bus_ids,
        std::vector<persistence::session::fx_chain> const& fx_chain_data)
{
    for (std::size_t i :
         range::indices(std::min(fx_chain_data.size(), bus_ids.size())))
    {
        mixer::bus_id fx_chain_bus = bus_ids[i];
        for (auto const& fx_plug : fx_chain_data[i])
        {
            batch.push_back(std::visit(
                    make_add_fx_module_action{fx_chain_bus},
                    fx_plug.as_variant()));
        }
    }
}

template <class GetState>
auto
create_fx_chains(persistence::session const& session, GetState&& get_state)
        -> batch_action
{
    batch_action action;

    state const& st = get_state();

    make_add_fx_module_actions(action, st.mixer_state.inputs, session.inputs);
    make_add_fx_module_actions(action, st.mixer_state.outputs, session.outputs);

    return action;
}

} // namespace

auto
apply_session(persistence::session session) -> thunk_action
{
    return [s = std::move(session)](auto&& get_state, auto&& dispatch) {
        std::invoke(
                std::forward<decltype(dispatch)>(dispatch),
                create_fx_chains(
                        s,
                        std::forward<decltype(get_state)>(get_state)));
    };
}

} // namespace piejam::runtime::actions
