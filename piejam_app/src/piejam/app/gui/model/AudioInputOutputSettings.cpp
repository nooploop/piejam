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

#include <piejam/app/gui/model/BusConfig.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/add_bus.h>
#include <piejam/runtime/actions/delete_bus.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/actions/select_bus_channel.h>
#include <piejam/runtime/actions/set_bus_name.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/audio_state_selectors.h>

#include <string>

namespace piejam::app::gui::model
{

AudioInputOutputSettings::AudioInputOutputSettings(
        store_dispatch store_dispatch,
        subscriber& state_change_subscriber,
        audio::bus_direction const settings_type)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_settings_type(settings_type)
{
}

void
AudioInputOutputSettings::subscribeStep(
        subscriber& state_change_subscriber,
        subscriptions_manager& subs,
        subscription_id subs_id)
{
    namespace selectors = runtime::audio_state_selectors;

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::make_num_device_channels_selector(m_settings_type),
            [this](std::size_t const num_input_channels) {
                QStringList channels;
                channels.push_back("-");
                for (std::size_t n = 0; n < num_input_channels; ++n)
                    channels.push_back(QString::number(n + 1));

                setChannels(channels);
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::make_bus_list_selector(m_settings_type),
            [this, &state_change_subscriber](
                    container::box<runtime::mixer::bus_list_t> bus_ids) {
                while (bus_ids->size() > numBusConfigs())
                {
                    auto const bus = numBusConfigs();
                    busConfigs()->addBusConfig(std::make_unique<BusConfig>(
                            dispatch(),
                            state_change_subscriber,
                            BusConfigSelectors{
                                    selectors::make_bus_name_selector(
                                            m_settings_type,
                                            bus),
                                    selectors::make_bus_type_selector(
                                            m_settings_type,
                                            bus),
                                    selectors::make_bus_channel_selector(
                                            m_settings_type,
                                            bus,
                                            audio::bus_channel::mono),
                                    selectors::make_bus_channel_selector(
                                            m_settings_type,
                                            bus,
                                            audio::bus_channel::left),
                                    selectors::make_bus_channel_selector(
                                            m_settings_type,
                                            bus,
                                            audio::bus_channel::right)}));
                }

                while (bus_ids->size() < numBusConfigs())
                {
                    busConfigs()->removeBusConfig();
                }
            });
}

template <audio::bus_direction D>
auto
makeSetBusNameAction(unsigned const bus, QString const& name)
{
    runtime::actions::set_bus_name<D> action;
    action.bus = bus;
    action.name = name.toStdString();
    return action;
}

void
AudioInputOutputSettings::setBusName(unsigned const bus, QString const& name)
{
    switch (m_settings_type)
    {
        case audio::bus_direction::input:
            dispatch(makeSetBusNameAction<audio::bus_direction::input>(
                    bus,
                    name));
            break;

        case audio::bus_direction::output:
            dispatch(makeSetBusNameAction<audio::bus_direction::output>(
                    bus,
                    name));
            break;
    }
}

void
AudioInputOutputSettings::selectChannel(
        audio::bus_channel const bc,
        unsigned const bus,
        unsigned const ch)
{
    runtime::actions::select_bus_channel action;
    action.direction = m_settings_type;
    action.bus = bus;
    action.channel_selector = bc;
    action.channel_index = static_cast<std::size_t>(ch) - 1;
    dispatch(action);
}

void
AudioInputOutputSettings::selectMonoChannel(
        unsigned const bus,
        unsigned const ch)
{
    selectChannel(audio::bus_channel::mono, bus, ch);
}

void
AudioInputOutputSettings::selectStereoLeftChannel(
        unsigned const bus,
        unsigned const ch)
{
    selectChannel(audio::bus_channel::left, bus, ch);
}

void
AudioInputOutputSettings::selectStereoRightChannel(
        unsigned const bus,
        unsigned const ch)
{
    selectChannel(audio::bus_channel::right, bus, ch);
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

void
AudioInputOutputSettings::deleteBus(unsigned const bus)
{
    runtime::actions::delete_bus action;
    action.direction = m_settings_type;
    action.bus = bus;
    dispatch(action);
}

} // namespace piejam::app::gui::model
