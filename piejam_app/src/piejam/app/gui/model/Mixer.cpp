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

#include <piejam/app/gui/model/Mixer.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/generic_list_model_edit_script_executor.h>
#include <piejam/app/gui/model/MixerChannel.h>
#include <piejam/audio/types.h>
#include <piejam/redux/store.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/request_mixer_levels_update.h>
#include <piejam/runtime/actions/set_bus_solo.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <boost/range/algorithm_ext/push_back.hpp>

namespace piejam::app::gui::model
{

Mixer::Mixer(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
Mixer::subscribe_step()
{
    namespace selectors = runtime::selectors;

    observe(selectors::make_bus_list_selector(audio::bus_direction::input),
            [this](container::box<runtime::mixer::bus_list_t> const& bus_ids) {
                generic_list_model_edit_script_executor<
                        piejam::gui::model::MixerChannel,
                        MixerChannel>
                        visitor{*inputChannels(),
                                dispatch(),
                                state_change_subscriber()};

                algorithm::apply_edit_script(
                        algorithm::edit_script(m_inputs, *bus_ids),
                        visitor);

                m_all = m_inputs = bus_ids;
                boost::push_back(m_all, m_outputs);
            });

    observe(selectors::make_bus_list_selector(audio::bus_direction::output),
            [this](container::box<runtime::mixer::bus_list_t> const& bus_ids) {
                generic_list_model_edit_script_executor<
                        piejam::gui::model::MixerChannel,
                        MixerChannel>
                        visitor{*outputChannels(),
                                dispatch(),
                                state_change_subscriber()};

                algorithm::apply_edit_script(
                        algorithm::edit_script(m_outputs, *bus_ids),
                        visitor);

                m_all = m_outputs = bus_ids;
                boost::push_back(m_all, m_inputs);
            });

    observe(selectors::select_input_solo_active,
            [this](bool const input_solo_active) {
                setInputSoloActive(input_solo_active);
            });
}

void
Mixer::setInputSolo(unsigned const index)
{
    runtime::actions::set_input_bus_solo action;
    action.index = index;
    dispatch(action);
}

void
Mixer::requestLevelsUpdate()
{
    dispatch(runtime::actions::request_mixer_levels_update(m_all));
}

} // namespace piejam::app::gui::model
