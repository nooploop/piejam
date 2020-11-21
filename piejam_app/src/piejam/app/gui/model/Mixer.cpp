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
#include <piejam/runtime/actions/request_levels_update.h>
#include <piejam/runtime/actions/set_bus_mute.h>
#include <piejam/runtime/actions/set_bus_pan_balance.h>
#include <piejam/runtime/actions/set_bus_solo.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/audio_state_selectors.h>

namespace piejam::app::gui::model
{

Mixer::Mixer(store_dispatch store_dispatch, subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
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
            selectors::make_bus_list_selector(audio::bus_direction::input),
            [this, &state_change_subscriber](
                    container::box<runtime::mixer::bus_list_t> bus_ids) {
                auto const num_channels = bus_ids->size();

                for (std::size_t bus = numInputChannels(); bus < num_channels;
                     ++bus)
                {
                    inputChannels()->addMixerChannel(std::make_unique<
                                                     MixerChannel>(
                            dispatch(),
                            state_change_subscriber,
                            MixerChannelSelectors{
                                    selectors::make_bus_name_selector(
                                            piejam::audio::bus_direction::input,
                                            bus),
                                    selectors::make_input_pan_selector(bus),
                                    selectors::make_input_mute_selector(bus),
                                    selectors::make_input_solo_selector(bus),
                                    selectors::make_input_level_selector(bus)},
                            bus_ids.get()[bus]));
                }

                while (bus_ids->size() < numInputChannels())
                {
                    inputChannels()->removeMixerChannel();
                }
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::make_bus_list_selector(audio::bus_direction::output),
            [this, &state_change_subscriber](
                    container::box<runtime::mixer::bus_list_t> bus_ids) {
                auto const num_channels = bus_ids->size();

                for (std::size_t bus = numOutputChannels(); bus < num_channels;
                     ++bus)
                {
                    outputChannels()->addMixerChannel(std::make_unique<
                                                      MixerChannel>(
                            dispatch(),
                            state_change_subscriber,
                            MixerChannelSelectors{
                                    selectors::make_bus_name_selector(
                                            piejam::audio::bus_direction::
                                                    output,
                                            bus),
                                    selectors::make_output_balance_selector(
                                            bus),
                                    selectors::make_output_mute_selector(bus),
                                    selector<bool>{
                                            [](runtime::audio_state const&) {
                                                return false;
                                            }},
                                    selectors::make_output_level_selector(bus)},
                            bus_ids.get()[bus]));
                }

                while (bus_ids->size() < numOutputChannels())
                {
                    outputChannels()->removeMixerChannel();
                }
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_input_solo_active,
            [this](bool const input_solo_active) {
                setInputSoloActive(input_solo_active);
            });
}

void
Mixer::setInputChannelPan(unsigned const index, double const pan)
{
    runtime::actions::set_input_bus_pan_balance action;
    action.index = index;
    action.pan_balance = static_cast<float>(pan);
    dispatch(action);
}

void
Mixer::setInputChannelMute(unsigned const index, bool const mute)
{
    runtime::actions::set_input_bus_mute action;
    action.index = index;
    action.mute = mute;
    dispatch(action);
}

void
Mixer::setInputSolo(unsigned const index)
{
    runtime::actions::set_input_bus_solo action;
    action.index = index;
    dispatch(action);
}

void
Mixer::setOutputChannelBalance(unsigned const index, double const balance)
{
    runtime::actions::set_output_bus_balance action;
    action.index = index;
    action.pan_balance = static_cast<float>(balance);
    dispatch(action);
}

void
Mixer::setOutputChannelMute(unsigned const index, bool const mute)
{
    runtime::actions::set_output_bus_mute action;
    action.index = index;
    action.mute = mute;
    dispatch(action);
}

void
Mixer::requestLevelsUpdate()
{
    dispatch(runtime::actions::request_levels_update{});
}

} // namespace piejam::app::gui::model
