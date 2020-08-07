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

#include <piejam/app/gui/model/AudioInputSettings.h>

#include <piejam/gui/model/BusConfig.h>
#include <piejam/redux/store.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/device_actions.h>
#include <piejam/runtime/actions/set_bus_name.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/audio_state_selectors.h>

#include <string>

namespace piejam::app::gui::model
{

AudioInputSettings::AudioInputSettings(
        store& app_store,
        subscriber& state_change_subscriber)
    : m_store(app_store)
{
    namespace selectors = runtime::audio_state_selectors;

    auto const subs_id = get_next_sub_id();

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_num_input_channels,
            [this](std::size_t const num_input_channels) {
                QStringList channels;
                channels.push_back("-");
                for (std::size_t n = 0; n < num_input_channels; ++n)
                    channels.push_back(QString::number(n + 1));

                setChannels(channels);
            });

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_num_input_busses,
            [this, &state_change_subscriber, subs_id = get_next_sub_id()](
                    std::size_t const num_busses) {
                m_subs.erase(subs_id);

                std::vector<piejam::gui::model::BusConfig> busConfigs(
                        num_busses);

                for (std::size_t bus = 0; bus < num_busses; ++bus)
                {
                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_bus_name_selector(
                                    audio::bus_direction::input,
                                    bus),
                            [this, bus](container::boxed_string name) {
                                busConfig(bus).setName(
                                        QString::fromStdString(*name));
                            });

                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_bus_type_selector(
                                    audio::bus_direction::input,
                                    bus),
                            [this, bus](audio::bus_type const t) {
                                busConfig(bus).setMono(
                                        t == audio::bus_type::mono);
                            });

                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_bus_channel_selector(
                                    audio::bus_direction::input,
                                    bus,
                                    audio::bus_channel::mono),
                            [this, bus](std::size_t const ch) {
                                busConfig(bus).setMonoChannel(ch + 1);
                            });
                }

                setBusConfigs(std::move(busConfigs));
            });
}

void
AudioInputSettings::setBusName(unsigned const bus, QString const& name)
{
    runtime::actions::set_bus_name action;
    action.bus_direction = audio::bus_direction::input;
    action.bus = bus;
    action.name = name.toStdString();
    m_store.dispatch<runtime::actions::set_bus_name>(action);
}

void
AudioInputSettings::selectMonoChannel(unsigned const bus, unsigned const ch)
{
    runtime::actions::select_bus_channel action;
    action.direction = audio::bus_direction::input;
    action.bus = bus;
    action.channel_selector = audio::bus_channel::mono;
    action.channel_index = static_cast<std::size_t>(ch) - 1;
    m_store.dispatch<runtime::actions::select_bus_channel>(action);
}

void
AudioInputSettings::selectStereoLeftChannel(
        unsigned const /*bus*/,
        unsigned const /*ch*/)
{
}

void
AudioInputSettings::selectStereoRightChannel(
        unsigned const /*bus*/,
        unsigned const /*ch*/)
{
}

void
AudioInputSettings::addMonoBus()
{
    runtime::actions::add_device_bus action;
    action.direction = audio::bus_direction::input;
    action.type = audio::bus_type::mono;
    m_store.dispatch<runtime::actions::add_device_bus>(action);
}

void
AudioInputSettings::addStereoBus()
{
}

void
AudioInputSettings::deleteBus(unsigned const bus)
{
    runtime::actions::delete_device_bus action;
    action.bus = bus;
    m_store.dispatch<runtime::actions::delete_device_bus>(action);
}

} // namespace piejam::app::gui::model
