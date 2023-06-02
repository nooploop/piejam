// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Factory.h>

#include <piejam/gui/item/Scope.h>
#include <piejam/gui/item/Spectrum.h>
#include <piejam/gui/item/Waveform.h>
#include <piejam/gui/model/AudioDeviceSettings.h>
#include <piejam/gui/model/AudioInputOutputSettings.h>
#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/DbScaleData.h>
#include <piejam/gui/model/FxBrowser.h>
#include <piejam/gui/model/FxChain.h>
#include <piejam/gui/model/FxFilter.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/gui/model/FxScope.h>
#include <piejam/gui/model/FxSpectrum.h>
#include <piejam/gui/model/Info.h>
#include <piejam/gui/model/Log.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MidiDeviceConfig.h>
#include <piejam/gui/model/MidiInputSettings.h>
#include <piejam/gui/model/Mixer.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/model/MixerChannelEdit.h>
#include <piejam/gui/model/MixerChannelPerform.h>
#include <piejam/gui/model/MixerDbScales.h>
#include <piejam/gui/model/ScopeData.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/StereoChannel.h>
#include <piejam/gui/model/StringList.h>
#include <piejam/gui/model/TriggerSlope.h>

#include <mutex>

static void
initResources()
{
    Q_INIT_RESOURCE(piejam_gui_resources);
}

namespace piejam::gui::model
{

static void
runRegistration()
{
    initResources();

    qRegisterMetaType<piejam::gui::model::StringList*>();
    qRegisterMetaType<piejam::gui::model::AudioDeviceSettings*>();
    qRegisterMetaType<piejam::gui::model::AudioInputOutputSettings*>();
    qRegisterMetaType<piejam::gui::model::Mixer*>();
    qRegisterMetaType<piejam::gui::model::MixerChannel*>();
    qRegisterMetaType<piejam::gui::model::MixerChannelEdit*>();
    qRegisterMetaType<piejam::gui::model::MixerChannelPerform*>();
    qRegisterMetaType<piejam::gui::model::DbScaleData*>();
    qRegisterMetaType<piejam::gui::model::Info*>();
    qRegisterMetaType<piejam::gui::model::Log*>();
    qRegisterMetaType<piejam::gui::model::FxParameter*>();
    qRegisterMetaType<piejam::gui::model::FxChain*>();
    qRegisterMetaType<piejam::gui::model::FxBrowser*>();
    qRegisterMetaType<piejam::gui::model::MidiInputSettings*>();
    qRegisterMetaType<piejam::gui::model::MidiAssignable*>();
    qRegisterMetaType<piejam::gui::model::AudioStreamProvider*>();
    qRegisterMetaType<piejam::gui::model::AudioStreamListener*>();
    qRegisterMetaType<piejam::gui::model::FxModuleContent*>();
    qRegisterMetaType<piejam::gui::model::FxScope*>();
    qRegisterMetaType<piejam::gui::model::WaveformDataObject*>();
    qRegisterMetaType<piejam::gui::model::ScopeData*>();
    qRegisterMetaType<piejam::gui::model::FxSpectrum*>();
    qRegisterMetaType<piejam::gui::model::SpectrumData*>();
    qRegisterMetaType<piejam::gui::model::FxFilter*>();

    qRegisterMetaType<piejam::gui::item::SpectrumScaleLabel>();

    qmlRegisterUncreatableType<piejam::gui::model::MixerChannelEdit>(
            "PieJam.Models",
            1,
            0,
            "MixerChannelEdit",
            "Not creatable");

    qmlRegisterUncreatableType<piejam::gui::model::FxModuleContent>(
            "PieJam.Models",
            1,
            0,
            "FxModuleContent",
            "Not createable");

    qmlRegisterUncreatableType<piejam::gui::model::FxScope>(
            "PieJam.Models",
            1,
            0,
            "FxScope",
            "Not createable");

    qmlRegisterType<piejam::gui::item::Waveform>(
            "PieJam.Items",
            1,
            0,
            "Waveform");
    qmlRegisterType<piejam::gui::item::Spectrum>(
            "PieJam.Items",
            1,
            0,
            "Spectrum");
    qmlRegisterType<piejam::gui::item::Scope>("PieJam.Items", 1, 0, "Scope");

    qRegisterMetaType<piejam::gui::model::TriggerSlope>();
    qmlRegisterUncreatableType<piejam::gui::model::TriggerSlopeClass>(
            "PieJam.Models",
            1,
            0,
            "TriggerSlope",
            "Not creatable as it is an enum type");

    qRegisterMetaType<piejam::gui::model::StereoChannel>();
    qmlRegisterUncreatableType<piejam::gui::model::StereoChannelClass>(
            "PieJam.Models",
            1,
            0,
            "StereoChannel",
            "Not creatable as it is an enum type");

    qRegisterMetaType<piejam::gui::model::BusType>();
    qmlRegisterUncreatableType<piejam::gui::model::BusTypeClass>(
            "PieJam.Models",
            1,
            0,
            "BusType",
            "Not creatable as it is an enum type");

    qmlRegisterSingletonInstance<piejam::gui::model::MixerDbScales>(
            "PieJam.Models",
            1,
            0,
            "MixerDbScales",
            &g_mixerDbScales);
}

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
    , m_log(std::make_unique<Log>(dispatch, state_change_subscriber))
    , m_fxBrowser(
              std::make_unique<FxBrowser>(dispatch, state_change_subscriber))
{
    static std::once_flag s_registered;
    std::call_once(s_registered, &runRegistration);
}

Factory::~Factory() = default;

} // namespace piejam::gui::model
