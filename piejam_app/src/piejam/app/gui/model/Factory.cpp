// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/Factory.h>

#include <piejam/app/gui/model/AudioDeviceSettings.h>
#include <piejam/app/gui/model/AudioInputOutputSettings.h>
#include <piejam/app/gui/model/FxBrowser.h>
#include <piejam/app/gui/model/FxChain.h>
#include <piejam/app/gui/model/Info.h>
#include <piejam/app/gui/model/MidiInputSettings.h>
#include <piejam/app/gui/model/Mixer.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/MidiDeviceConfig.h>
#include <piejam/gui/model/MixerChannel.h>

#include <mutex>

static void
init_resources()
{
    Q_INIT_RESOURCE(piejam_app_resources);
}

namespace piejam::app::gui::model
{

Factory::Factory(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber)
    : m_store_dispatch(dispatch)
    , m_state_change_subscriber(state_change_subscriber)
    , m_audioDeviceSettings(std::make_unique<AudioDeviceSettings>(
              dispatch,
              state_change_subscriber))
    , m_audioInputSettings(std::make_unique<AudioInputSettings>(
              dispatch,
              state_change_subscriber))
    , m_audioOutputSettings(std::make_unique<AudioOutputSettings>(
              dispatch,
              state_change_subscriber))
    , m_midiInputSettings(std::make_unique<MidiInputSettings>(
              dispatch,
              state_change_subscriber))
    , m_mixer(std::make_unique<Mixer>(dispatch, state_change_subscriber))
    , m_info(std::make_unique<Info>(dispatch, state_change_subscriber))
    , m_fxChain(std::make_unique<FxChain>(dispatch, state_change_subscriber))
    , m_fxBrowser(
              std::make_unique<FxBrowser>(dispatch, state_change_subscriber))
{
    static std::once_flag s_resources_initialized;
    std::call_once(s_resources_initialized, &init_resources);
}

Factory::~Factory() = default;

} // namespace piejam::app::gui::model
