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

#include <piejam/app/gui/model/FxChain.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/algorithm/index_of.h>
#include <piejam/app/gui/generic_list_model_edit_script_executor.h>
#include <piejam/app/gui/model/BusName.h>
#include <piejam/app/gui/model/FxModule.h>
#include <piejam/functional/overload.h>
#include <piejam/runtime/actions/select_fx_chain_bus.h>
#include <piejam/runtime/selectors.h>

#include <boost/range/algorithm_ext/push_back.hpp>

namespace piejam::app::gui::model
{

FxChain::FxChain(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
FxChain::subscribe_step()
{
    observe(runtime::selectors::make_bus_list_selector(
                    audio::bus_direction::input),
            [this](container::box<runtime::mixer::bus_list_t> const& bus_ids) {
                updateBuses(audio::bus_direction::input, bus_ids);
            });

    observe(runtime::selectors::make_bus_list_selector(
                    audio::bus_direction::output),
            [this](container::box<runtime::mixer::bus_list_t> const& bus_ids) {
                updateBuses(audio::bus_direction::output, bus_ids);
            });

    observe(runtime::selectors::select_fx_chain_bus,
            [this](runtime::mixer::bus_id const& fx_chain_bus) {
                setSelectedBus(static_cast<int>(
                        algorithm::index_of(m_all, fx_chain_bus)));
            });

    observe(runtime::selectors::select_current_fx_chain,
            [this](container::box<runtime::fx::chain_t> const& fx_chain) {
                generic_list_model_edit_script_executor<
                        piejam::gui::model::FxModule,
                        FxModule>
                        visitor{*modules(),
                                dispatch(),
                                state_change_subscriber()};

                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_fx_chain, *fx_chain),
                        visitor);

                m_fx_chain = fx_chain;
            });
}

void
FxChain::updateBuses(
        audio::bus_direction bus_dir,
        container::box<runtime::mixer::bus_list_t> const& bus_ids)
{
    (bus_dir == audio::bus_direction::input ? m_inputs : m_outputs) = bus_ids;

    auto visitor = overload{
            [this](algorithm::edit_script_deletion const& del) {
                buses()->remove(del.pos);
            },
            [this](algorithm::edit_script_insertion<
                    runtime::mixer::bus_id> const& ins) {
                auto busName = std::make_unique<BusName>(
                        dispatch(),
                        state_change_subscriber(),
                        ins.value);

                connect(*busName);

                buses()->add(ins.pos, std::move(busName));
            }};

    runtime::mixer::bus_list_t all;
    all.reserve(m_inputs->size() + m_outputs->size());
    boost::push_back(all, *m_inputs);
    boost::push_back(all, *m_outputs);

    algorithm::apply_edit_script(algorithm::edit_script(m_all, all), visitor);

    m_all = all;
}

void
FxChain::selectBus(int pos)
{
    runtime::actions::select_fx_chain_bus action;

    if (auto const index = static_cast<std::size_t>(pos); index < m_all.size())
    {
        action.bus_id = m_all[index];
    }

    dispatch(action);
}

} // namespace piejam::app::gui::model
