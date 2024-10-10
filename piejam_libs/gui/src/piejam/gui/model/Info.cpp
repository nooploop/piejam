// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Info.h>

#include <piejam/redux/subscriptions_manager.h>
#include <piejam/runtime/actions/recording.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

Info::Info(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
Info::onSubscribe()
{
    observe(runtime::selectors::select_recording,
            [this](bool const x) { setRecording(x); });

    observe(runtime::selectors::select_xruns, [this](std::size_t const xruns) {
        setXruns(static_cast<unsigned>(xruns));
    });

    observe(runtime::selectors::select_cpu_load,
            [this](float const cpu_load) { setAudioLoad(cpu_load); });

    observe(runtime::selectors::select_midi_learning,
            [this](bool const midi_learning) { setMidiLearn(midi_learning); });

    requestUpdates(std::chrono::milliseconds{40}, [this]() {
        dispatch(runtime::actions::request_info_update{});
    });
}

void
Info::changeRecording(bool const rec)
{
    emit recordingChanged();

    if (rec)
    {
        dispatch(runtime::actions::start_recording{});
    }
    else
    {
        dispatch(runtime::actions::stop_recording{});
    }
}

} // namespace piejam::gui::model
