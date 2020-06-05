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

#include <piejam/redux/store.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/engine_actions.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/audio_state_selectors.h>

namespace piejam::app::gui::model
{

Mixer::Mixer(store& app_store, subscriber& state_change_subscriber)
    : m_store(app_store)
{
    namespace selectors = runtime::audio_state_selectors;

    auto const subs_id = get_next_sub_id();

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_num_input_devices,
            [this, &state_change_subscriber, subs_id = get_next_sub_id()](
                    std::size_t const num_input_devices) mutable {
                m_subs.erase(subs_id);

                setNumInputChannels(num_input_devices);

                for (std::size_t i = 0; i < num_input_devices; ++i)
                {
                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_input_gain_selector(i),
                            [this, i](float x) { inputChannel(i).setGain(x); });

                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_input_level_selector(i),
                            [this, i](float x) {
                                inputChannel(i).setLevel(x);
                            });

                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_input_enabled_selector(i),
                            [this, i](bool x) {
                                inputChannel(i).setEnabled(x);
                            });
                }
            });

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            runtime::audio_state_selectors::select_output_gain,
            [this](float x) { outputChannel()->setGain(x); });

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            runtime::audio_state_selectors::select_output_level,
            [this](float x) { outputChannel()->setLevel(x); });
}

void
Mixer::toggleInputChannel(unsigned const index)
{
    runtime::actions::toggle_input_channel action;
    action.index = index;
    m_store.dispatch<runtime::actions::toggle_input_channel>(action);
}

void
Mixer::setInputChannelGain(unsigned const index, double const gain)
{
    runtime::actions::set_input_channel_gain action;
    action.index = index;
    action.gain = static_cast<float>(gain);
    m_store.dispatch<runtime::actions::set_input_channel_gain>(action);
}

void
Mixer::setOutputChannelGain(double const gain)
{
    runtime::actions::set_output_channel_gain action;
    action.gain = static_cast<float>(gain);
    m_store.dispatch<runtime::actions::set_output_channel_gain>(action);
}

void
Mixer::requestLevelsUpdate()
{
    m_store.dispatch<runtime::actions::request_levels_update>();
}

} // namespace piejam::app::gui::model
