// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioDeviceSettings.h>

#include <piejam/algorithm/index_of.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/gui/model/StringList.h>
#include <piejam/redux/store.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/actions/refresh_devices.h>
#include <piejam/runtime/actions/select_period_count.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_sample_rate.h>
#include <piejam/runtime/selectors.h>

#include <boost/hof/compose.hpp>

#include <algorithm>

namespace piejam::gui::model
{

static auto
deviceNames(std::vector<piejam::audio::pcm_descriptor> const& devices)
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
toQStringList(Vector const& l) -> QStringList
{
    QStringList result;
    std::ranges::transform(
            l,
            std::back_inserter(result),
            [](auto const x) -> QString { return QString::number(x); });
    return result;
}

struct AudioDeviceSettings::Impl
{
    StringList inputDevices;
    StringList outputDevices;
    StringList sampleRates;
    StringList periodSizes;
    StringList periodCounts;
};

AudioDeviceSettings::AudioDeviceSettings(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>())
{
}

AudioDeviceSettings::~AudioDeviceSettings() = default;

auto
AudioDeviceSettings::inputDevices() -> StringList*
{
    return &m_impl->inputDevices;
}

auto
AudioDeviceSettings::outputDevices() -> StringList*
{
    return &m_impl->outputDevices;
}

auto
AudioDeviceSettings::sampleRates() -> StringList*
{
    return &m_impl->sampleRates;
}

auto
AudioDeviceSettings::periodSizes() -> StringList*
{
    return &m_impl->periodSizes;
}

auto
AudioDeviceSettings::periodCounts() -> StringList*
{
    return &m_impl->periodCounts;
}

void
AudioDeviceSettings::onSubscribe()
{
    namespace selectors = runtime::selectors;

    observe(selectors::select_input_devices,
            [this](selectors::input_devices const& input_devices) {
                inputDevices()->setElements(
                        deviceNames(input_devices.first->inputs));
                inputDevices()->setFocused(
                        static_cast<int>(input_devices.second));
            });

    observe(selectors::select_output_devices,
            [this](selectors::output_devices const& output_devices) {
                outputDevices()->setElements(
                        deviceNames(output_devices.first->outputs));
                outputDevices()->setFocused(
                        static_cast<int>(output_devices.second));
            });

    observe(selectors::select_sample_rate,
            [this](selectors::sample_rate const& sample_rate) {
                auto const index = algorithm::index_of(
                        *sample_rate.first,
                        sample_rate.second);

                sampleRates()->setElements(toQStringList(*sample_rate.first));
                sampleRates()->setFocused(static_cast<int>(index));
            });

    observe(selectors::select_period_size,
            [this](selectors::period_size const& period_size) {
                auto const index = algorithm::index_of(
                        *period_size.first,
                        period_size.second);

                periodSizes()->setElements(toQStringList(*period_size.first));
                periodSizes()->setFocused(static_cast<int>(index));
            });

    observe(selectors::select_period_count,
            [this](selectors::period_count const& period_count) {
                auto const index = algorithm::index_of(
                        *period_count.first,
                        period_count.second);

                periodCounts()->setElements(toQStringList(*period_count.first));
                periodCounts()->setFocused(static_cast<int>(index));
            });

    observe(selectors::select_buffer_latency,
            [this](float const x) { setBufferLatency(x); });
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
AudioDeviceSettings::selectSamplerate(unsigned const index)
{
    runtime::actions::select_sample_rate action;
    action.index = index;
    dispatch(action);
}

void
AudioDeviceSettings::selectPeriodSize(unsigned const index)
{
    runtime::actions::select_period_size action;
    action.index = index;
    dispatch(action);
}

void
AudioDeviceSettings::selectPeriodCount(unsigned const index)
{
    runtime::actions::select_period_count action;
    action.index = index;
    dispatch(action);
}

} // namespace piejam::gui::model
