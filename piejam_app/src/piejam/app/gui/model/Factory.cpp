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
#include <piejam/gui/item/Scope.h>
#include <piejam/gui/item/Spectrum.h>
#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/gui/model/FxScope.h>
#include <piejam/gui/model/FxSpectrum.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MidiDeviceConfig.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/model/MixerChannelEdit.h>
#include <piejam/gui/model/MixerChannelPerform.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/StereoChannel.h>

#include <QQmlEngine>

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
    qRegisterMetaType<piejam::gui::model::AudioDeviceSettings*>();
    qRegisterMetaType<piejam::gui::model::AudioInputOutputSettings*>();
    qRegisterMetaType<piejam::gui::model::Mixer*>();
    qRegisterMetaType<piejam::gui::model::MixerChannel*>();
    qRegisterMetaType<piejam::gui::model::MixerChannelEdit*>();
    qRegisterMetaType<piejam::gui::model::MixerChannelPerform*>();
    qRegisterMetaType<piejam::gui::model::Info*>();
    qRegisterMetaType<piejam::gui::model::FxChain*>();
    qRegisterMetaType<piejam::gui::model::FxBrowser*>();
    qRegisterMetaType<piejam::gui::model::MidiInputSettings*>();
    qRegisterMetaType<piejam::gui::model::MidiAssignable*>();
    qRegisterMetaType<piejam::gui::model::AudioStreamProvider*>();
    qRegisterMetaType<piejam::gui::model::AudioStreamListener*>();
    qRegisterMetaType<piejam::gui::model::FxModuleContent*>();
    qRegisterMetaType<piejam::gui::model::FxScope*>();
    qRegisterMetaType<piejam::gui::model::ScopeLinesObject*>();
    qRegisterMetaType<piejam::gui::model::FxSpectrum*>();
    qRegisterMetaType<piejam::gui::model::SpectrumData*>();

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

    qmlRegisterType<piejam::gui::item::Scope>("PieJam.Items", 1, 0, "Scope");
    qmlRegisterType<piejam::gui::item::Spectrum>(
            "PieJam.Items",
            1,
            0,
            "Spectrum");

    qRegisterMetaType<piejam::gui::model::StereoChannel>();
    qmlRegisterUncreatableType<piejam::gui::model::StereoChannelClass>(
            "PieJam.Models",
            1,
            0,
            "StereoChannel",
            "Not creatable as it is an enum type");
}

Factory::~Factory() = default;

} // namespace piejam::app::gui::model
