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

#include <piejam/app/gui/model/AudioDeviceSettings.h>

#include <piejam/algorithm/index_of.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/redux/store.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/actions/refresh_devices.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_samplerate.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/selectors.h>

#include <boost/hof/compose.hpp>

#include <algorithm>

namespace piejam::app::gui::model
{

static auto
device_names(std::vector<piejam::audio::pcm_descriptor> const& devices)
        -> QStringList
{
    QStringList result;
    std::ranges::transform(
            devices,
            std::back_inserter(result),
            boost::hof::compose(
                    &QString::fromStdString,
                    &piejam::audio::pcm_descriptor::name));
    return result;
}

template <class Vector>
static auto
to_QStringList(Vector const& l) -> QStringList
{
    QStringList result;
    std::ranges::transform(
            l,
            std::back_inserter(result),
            [](auto const x) -> QString { return QString::number(x); });
    return result;
}

AudioDeviceSettings::AudioDeviceSettings(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
AudioDeviceSettings::subscribeStep(
        runtime::subscriber& state_change_subscriber,
        runtime::subscriptions_manager& subs,
        runtime::subscription_id subs_id)
{
    namespace selectors = runtime::selectors;

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_input_devices,
            [this](selectors::input_devices const& input_devices) {
                inputDevices()->setElements(
                        device_names(input_devices.first->inputs));
                inputDevices()->setFocused(
                        static_cast<int>(input_devices.second));
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_output_devices,
            [this](selectors::output_devices const& output_devices) {
                outputDevices()->setElements(
                        device_names(output_devices.first->outputs));
                outputDevices()->setFocused(
                        static_cast<int>(output_devices.second));
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_samplerate,
            [this](selectors::samplerate const& samplerate) {
                auto const index = algorithm::index_of(
                        *samplerate.first,
                        samplerate.second);

                samplerates()->setElements(to_QStringList(*samplerate.first));
                samplerates()->setFocused(static_cast<int>(index));
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            selectors::select_period_size,
            [this](selectors::period_size const& period_size) {
                auto const index = algorithm::index_of(
                        *period_size.first,
                        period_size.second);

                periodSizes()->setElements(to_QStringList(*period_size.first));
                periodSizes()->setFocused(static_cast<int>(index));
            });
}

void
AudioDeviceSettings::refreshDeviceLists()
{
    dispatch(runtime::actions::refresh_devices{});
}

void
AudioDeviceSettings::selectInputDevice(unsigned const index)
{
    runtime::actions::initiate_device_selection action;
    action.input = true;
    action.index = index;
    dispatch(action);
}

void
AudioDeviceSettings::selectOutputDevice(unsigned const index)
{
    runtime::actions::initiate_device_selection action;
    action.input = false;
    action.index = index;
    dispatch(action);
}

void
AudioDeviceSettings::selectSamplerate(unsigned index)
{
    runtime::actions::select_samplerate action;
    action.index = index;
    dispatch(action);
}

void
AudioDeviceSettings::selectPeriodSize(unsigned index)
{
    runtime::actions::select_period_size action;
    action.index = index;
    dispatch(action);
}

} // namespace piejam::app::gui::model
