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

#include <piejam/runtime/actions/replace_fx_module.h>

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <spdlog/spdlog.h>

namespace piejam::runtime::actions
{

auto
make_replace_fx_module_action(
        state const& st,
        mixer::bus_id const fx_chain_bus,
        std::size_t const position,
        fx::internal const fx_type) -> batch_action
{
    batch_action batch;

    mixer::bus const* const bus = (*st.mixer_state.buses)[fx_chain_bus];
    BOOST_ASSERT(bus);

    if (position < bus->fx_chain->size())
    {
        auto delete_action = std::make_unique<actions::delete_fx_module>();
        delete_action->fx_mod_id = (*bus->fx_chain)[position];
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
        mixer::bus_id const fx_chain_bus,
        std::size_t const position,
        fx::internal const fx_type) -> thunk_action
{
    return [=](auto&& get_state, auto&& dispatch) {
        dispatch(make_replace_fx_module_action(
                get_state(),
                fx_chain_bus,
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
                get_state().fx_chain_bus,
                position,
                fx_type));
    };
}

auto
make_replace_fx_module_action(
        state const& st,
        mixer::bus_id const fx_chain_bus,
        std::size_t const position,
        audio::ladspa::plugin_id_t const plugin_id,
        std::string_view const& name) -> batch_action
{
    batch_action batch;

    mixer::bus const* const bus = (*st.mixer_state.buses)[fx_chain_bus];
    BOOST_ASSERT(bus);

    if (position < bus->fx_chain->size())
    {
        auto delete_action = std::make_unique<actions::delete_fx_module>();
        delete_action->fx_mod_id = (*bus->fx_chain)[position];
        batch.push_back(std::move(delete_action));
    }

    auto insert_action = std::make_unique<actions::load_ladspa_fx_plugin>();
    insert_action->fx_chain_bus = fx_chain_bus;
    insert_action->position = position;
    insert_action->plugin_id = plugin_id;
    insert_action->name = name;
    batch.push_back(std::move(insert_action));

    return batch;
}

auto
replace_fx_module(
        mixer::bus_id const fx_chain_bus,
        std::size_t const position,
        audio::ladspa::plugin_id_t const plugin_id,
        std::string_view const& name) -> thunk_action
{
    return [=, name = std::string(name)](auto&& get_state, auto&& dispatch) {
        dispatch(make_replace_fx_module_action(
                get_state(),
                fx_chain_bus,
                position,
                plugin_id,
                name));
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
                get_state().fx_chain_bus,
                position,
                plugin_id,
                name));
    };
}

} // namespace piejam::runtime::actions
