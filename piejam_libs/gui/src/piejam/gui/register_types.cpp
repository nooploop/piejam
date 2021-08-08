// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/register_types.h>

#include <piejam/gui/item/Scope.h>
#include <piejam/gui/item/Spectrum.h>
#include <piejam/gui/model/AudioDeviceSettings.h>
#include <piejam/gui/model/AudioInputOutputSettings.h>
#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/gui/model/FxBrowser.h>
#include <piejam/gui/model/FxChain.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/gui/model/FxScope.h>
#include <piejam/gui/model/FxSpectrum.h>
#include <piejam/gui/model/Info.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MidiDeviceConfig.h>
#include <piejam/gui/model/MidiInputSettings.h>
#include <piejam/gui/model/Mixer.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/model/MixerChannelEdit.h>
#include <piejam/gui/model/MixerChannelPerform.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/StereoChannel.h>

#include <mutex>

static void
init_resources()
{
    Q_INIT_RESOURCE(piejam_gui_resources);
}

namespace piejam::gui
{

static void
run_registration()
{
    init_resources();

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

void
register_types()
{
    static std::once_flag s_registered;
    std::call_once(s_registered, &run_registration);
}

} // namespace piejam::gui
