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

#include <piejam/app/gui/model/MixerChannel.h>
#include <piejam/audio/types.h>
#include <piejam/functional/overload.h>
#include <piejam/redux/store.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/engine_actions.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/audio_state_selectors.h>

namespace piejam::app::gui::model
{

Mixer::Mixer(store& app_store, subscriber& state_change_subscriber)
    : base_t(state_change_subscriber)
    , m_store(app_store)
{
}

void
Mixer::subscribeStep(
        subscriber& state_change_subscriber,
        subscriptions_manager& subs,
        subscription_id subs_id)
{
    namespace selectors = runtime::audio_state_selectors;

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::make_num_busses_selector(audio::bus_direction::input),
            [this, &state_change_subscriber](std::size_t const num_busses) {
                while (num_busses > numInputChannels())
                {
                    auto const bus = numInputChannels();
                    inputChannels()->addMixerChannel(std::make_unique<
                                                     MixerChannel>(
                            state_change_subscriber,
                            MixerChannelSelectors{
                                    selectors::make_bus_name_selector(
                                            piejam::audio::bus_direction::input,
                                            bus),
                                    selectors::make_input_volume_selector(bus),
                                    selectors::make_input_pan_selector(bus),
                                    selectors::make_input_mute_selector(bus),
                                    selectors::make_input_level_selector(
                                            bus)}));
                }

                while (num_busses < numInputChannels())
                {
                    inputChannels()->removeMixerChannel();
                }
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::make_num_busses_selector(audio::bus_direction::output),
            [this, &state_change_subscriber](std::size_t const num_busses) {
                while (num_busses > numOutputChannels())
                {
                    auto const bus = numOutputChannels();
                    outputChannels()->addMixerChannel(std::make_unique<
                                                      MixerChannel>(
                            state_change_subscriber,
                            MixerChannelSelectors{
                                    selectors::make_bus_name_selector(
                                            piejam::audio::bus_direction::
                                                    output,
                                            bus),
                                    selectors::make_output_volume_selector(bus),
                                    selectors::make_output_balance_selector(
                                            bus),
                                    selectors::make_output_mute_selector(bus),
                                    selectors::make_output_level_selector(
                                            bus)}));
                }

                while (num_busses < numOutputChannels())
                {
                    outputChannels()->removeMixerChannel();
                }
            });
}

void
Mixer::setInputChannelVolume(unsigned const index, double const volume)
{
    runtime::actions::set_input_channel_volume action;
    action.index = index;
    action.volume = static_cast<float>(volume);
    m_store.dispatch<runtime::actions::set_input_channel_volume>(action);
}

void
Mixer::setInputChannelPan(unsigned const index, double const pan)
{
    runtime::actions::set_input_channel_pan action;
    action.index = index;
    action.pan = static_cast<float>(pan);
    m_store.dispatch<runtime::actions::set_input_channel_pan>(action);
}

void
Mixer::setInputChannelMute(unsigned const index, bool const mute)
{
    runtime::actions::set_input_channel_mute action;
    action.index = index;
    action.mute = mute;
    m_store.dispatch<runtime::actions::set_input_channel_mute>(action);
}

void
Mixer::setOutputChannelVolume(unsigned const index, double const volume)
{
    runtime::actions::set_output_channel_volume action;
    action.index = index;
    action.volume = static_cast<float>(volume);
    m_store.dispatch<runtime::actions::set_output_channel_volume>(action);
}

void
Mixer::setOutputChannelBalance(unsigned const index, double const balance)
{
    runtime::actions::set_output_channel_balance action;
    action.index = index;
    action.balance = static_cast<float>(balance);
    m_store.dispatch<runtime::actions::set_output_channel_balance>(action);
}

void
Mixer::setOutputChannelMute(unsigned const index, bool const mute)
{
    runtime::actions::set_output_channel_mute action;
    action.index = index;
    action.mute = mute;
    m_store.dispatch<runtime::actions::set_output_channel_mute>(action);
}

void
Mixer::requestLevelsUpdate()
{
    m_store.dispatch<runtime::actions::request_levels_update>();
}

} // namespace piejam::app::gui::model
