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

#include <piejam/app/gui/model/AudioInputOutputSettings.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/generic_list_model_edit_script_executor.h>
#include <piejam/app/gui/model/BusConfig.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/add_bus.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/selectors.h>

#include <string>

namespace piejam::app::gui::model
{

AudioInputOutputSettings::AudioInputOutputSettings(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        audio::bus_direction const settings_type)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_settings_type(settings_type)
{
}

void
AudioInputOutputSettings::subscribe_step()
{
    namespace selectors = runtime::selectors;

    observe(selectors::make_num_device_channels_selector(m_settings_type),
            [this](std::size_t const num_input_channels) {
                QStringList channels;
                channels.push_back("-");
                for (std::size_t n = 0; n < num_input_channels; ++n)
                    channels.push_back(QString::number(n + 1));

                setChannels(channels);
            });

    observe(selectors::make_bus_list_selector(m_settings_type),
            [this](container::box<runtime::mixer::bus_list_t> const& bus_ids) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(m_bus_ids, *bus_ids),
                        generic_list_model_edit_script_executor{
                                *busConfigs(),
                                [this](runtime::mixer::bus_id bus_id) {
                                    return std::make_unique<BusConfig>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            bus_id);
                                }});

                m_bus_ids = bus_ids;
            });
}

void
AudioInputOutputSettings::addBus(audio::bus_type bus_type)
{
    runtime::actions::add_bus action;
    action.direction = m_settings_type;
    action.type = bus_type;
    dispatch(action);
}

void
AudioInputOutputSettings::addMonoBus()
{
    addBus(audio::bus_type::mono);
}

void
AudioInputOutputSettings::addStereoBus()
{
    addBus(audio::bus_type::stereo);
}

} // namespace piejam::app::gui::model
