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

#include <piejam/audio/bus_defs.h>
#include <piejam/functional/overload.h>
#include <piejam/redux/store.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/engine_actions.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/audio_state_selectors.h>

namespace piejam::app::gui::model
{

static void
set_channel_level(
        piejam::gui::model::MixerChannel& mixerChannel,
        audio::mixer::stereo_level const& level)
{
    mixerChannel.setLevel(level.left, level.right);
}

static void
set_input_channel_level(
        Mixer& m,
        std::size_t ch,
        audio::mixer::stereo_level const& level)
{
    set_channel_level(m.inputChannel(ch), level);
}

static void
set_output_channel_level(Mixer& m, audio::mixer::stereo_level const& level)
{
    set_channel_level(*m.outputChannel(), level);
}

Mixer::Mixer(store& app_store, subscriber& state_change_subscriber)
    : m_store(app_store)
{
    namespace selectors = runtime::audio_state_selectors;

    auto const subs_id = get_next_sub_id();

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_num_input_busses,
            [this, &state_change_subscriber, subs_id = get_next_sub_id()](
                    std::size_t const num_input_busses) mutable {
                m_subs.erase(subs_id);

                setNumInputChannels(num_input_busses);

                for (std::size_t bus = 0; bus < num_input_busses; ++bus)
                {
                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_bus_name_selector(
                                    audio::bus_direction::input,
                                    bus),
                            [this, bus](container::boxed_string name) {
                                inputChannel(bus).setName(
                                        QString::fromStdString(*name));
                            });

                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_input_gain_selector(bus),
                            [this, bus](float x) {
                                inputChannel(bus).setGain(x);
                            });

                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_input_pan_selector(bus),
                            [this, bus](float x) {
                                inputChannel(bus).setPanBalance(x);
                            });

                    m_subs.observe(
                            subs_id,
                            state_change_subscriber,
                            selectors::make_input_level_selector(bus),
                            [this, bus](audio::mixer::stereo_level const& x) {
                                set_input_channel_level(*this, bus, x);
                            });
                }
            });

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::make_bus_name_selector(audio::bus_direction::output, 0),
            [this](container::boxed_string name) {
                outputChannel()->setName(QString::fromStdString(*name));
            });

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_output_gain,
            [this](float x) { outputChannel()->setGain(x); });

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_output_balance,
            [this](float const x) { outputChannel()->setPanBalance(x); });

    m_subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_output_level,
            [this](audio::mixer::stereo_level const& x) {
                set_output_channel_level(*this, x);
            });
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
Mixer::setInputChannelPan(unsigned const index, double const pan)
{
    runtime::actions::set_input_channel_pan action;
    action.index = index;
    action.pan = static_cast<float>(pan);
    m_store.dispatch<runtime::actions::set_input_channel_pan>(action);
}

void
Mixer::setOutputChannelGain(double const gain)
{
    runtime::actions::set_output_channel_gain action;
    action.gain = static_cast<float>(gain);
    m_store.dispatch<runtime::actions::set_output_channel_gain>(action);
}

void
Mixer::setOutputChannelBalance(double const balance)
{
    runtime::actions::set_output_channel_balance action;
    action.balance = static_cast<float>(balance);
    m_store.dispatch<runtime::actions::set_output_channel_balance>(action);
}

void
Mixer::requestLevelsUpdate()
{
    m_store.dispatch<runtime::actions::request_levels_update>();
}

} // namespace piejam::app::gui::model
