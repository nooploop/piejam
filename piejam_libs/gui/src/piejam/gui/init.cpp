// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/DbScale.h>
#include <piejam/gui/item/FixedLinearScaleGrid.h>
#include <piejam/gui/item/FixedLogScaleGrid.h>
#include <piejam/gui/item/Scope.h>
#include <piejam/gui/item/Spectrum.h>
#include <piejam/gui/item/Waveform.h>
#include <piejam/gui/model/AudioDeviceSettings.h>
#include <piejam/gui/model/AudioInputOutputSettings.h>
#include <piejam/gui/model/AudioRouting.h>
#include <piejam/gui/model/AudioRoutingSelection.h>
#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/AuxChannel.h>
#include <piejam/gui/model/AuxSend.h>
#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/DbScaleData.h>
#include <piejam/gui/model/EnumListModel.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/ExternalAudioDeviceConfig.h>
#include <piejam/gui/model/FileDialog.h>
#include <piejam/gui/model/FileDialogEntry.h>
#include <piejam/gui/model/FilePath.h>
#include <piejam/gui/model/Filesystem.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxBrowser.h>
#include <piejam/gui/model/FxChainModule.h>
#include <piejam/gui/model/FxGenericModule.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/FxModuleRegistry.h>
#include <piejam/gui/model/FxModuleView.h>
#include <piejam/gui/model/Info.h>
#include <piejam/gui/model/IntParameter.h>
#include <piejam/gui/model/Log.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MidiDeviceConfig.h>
#include <piejam/gui/model/MidiInputSettings.h>
#include <piejam/gui/model/Mixer.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/model/MixerChannelAdd.h>
#include <piejam/gui/model/MixerChannelAuxSend.h>
#include <piejam/gui/model/MixerChannelEdit.h>
#include <piejam/gui/model/MixerChannelFx.h>
#include <piejam/gui/model/MixerChannelModels.h>
#include <piejam/gui/model/MixerChannelPerform.h>
#include <piejam/gui/model/MixerDbScales.h>
#include <piejam/gui/model/Parameter.h>
#include <piejam/gui/model/Root.h>
#include <piejam/gui/model/ScopeSlot.h>
#include <piejam/gui/model/SessionSettings.h>
#include <piejam/gui/model/SpectrumSlot.h>
#include <piejam/gui/model/StereoLevel.h>
#include <piejam/gui/model/String.h>
#include <piejam/gui/model/StringList.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/UISettings.h>
#include <piejam/gui/model/WaveformSlot.h>

#include <QDir>

#include <mutex>

static void
initResources()
{
    Q_INIT_RESOURCE(piejam_gui_resources);
}

namespace piejam::gui
{

namespace
{

// Helper macro to reduce boilerplate code for registering types.
// We use macros here so QtCreator can see those types in the QML editor.

#define PIEJAM_GUI_MODEL(type, name)                                           \
    qmlRegisterUncreatableType<type>("PieJam.Models", 1, 0, name, "C++ type")

#define PIEJAM_GUI_ITEM(type, name)                                            \
    qmlRegisterType<type>("PieJam.Items", 1, 0, name)

void
runRegistration()
{
    qRegisterMetaType<model::MaterialColor>("MaterialColor");

    PIEJAM_GUI_MODEL(model::AudioDeviceSettings, "AudioDeviceSettings");
    PIEJAM_GUI_MODEL(
        model::AudioInputOutputSettings,
        "AudioInputOutputSettings");
    PIEJAM_GUI_MODEL(model::AudioRouting, "AudioRouting");
    PIEJAM_GUI_MODEL(model::AudioRoutingSelection, "AudioRoutingSelection");
    PIEJAM_GUI_MODEL(model::AudioStreamProvider, "AudioStreamProvider");
    PIEJAM_GUI_MODEL(model::AuxChannel, "AuxChannel");
    PIEJAM_GUI_MODEL(model::AuxSend, "AuxSend");
    PIEJAM_GUI_MODEL(model::BoolParameter, "BoolParameter");
    PIEJAM_GUI_MODEL(model::DbScaleData, "DbScaleData");
    PIEJAM_GUI_MODEL(model::EnumListModel, "EnumListModel");
    PIEJAM_GUI_MODEL(model::EnumParameter, "EnumParameter");
    PIEJAM_GUI_MODEL(
        model::ExternalAudioDeviceConfig,
        "ExternalAudioDeviceConfig");
    PIEJAM_GUI_MODEL(model::FileDialog, "FileDialog");
    PIEJAM_GUI_MODEL(model::FileDialogEntry, "FileDialogEntry");
    PIEJAM_GUI_MODEL(model::FloatParameter, "FloatParameter");
    PIEJAM_GUI_MODEL(model::FxBrowser, "FxBrowser");
    PIEJAM_GUI_MODEL(model::FxChainModule, "FxChainModule");
    PIEJAM_GUI_MODEL(model::FxGenericModule, "FxGenericModule");
    PIEJAM_GUI_MODEL(model::FxModule, "FxModule");
    PIEJAM_GUI_MODEL(model::FxModuleRegistry, "FxModuleRegistry");
    PIEJAM_GUI_MODEL(model::FxModuleRegistryItem, "FxModuleRegistryItem");
    PIEJAM_GUI_MODEL(model::FxModuleType, "FxModuleType");
    PIEJAM_GUI_MODEL(model::FxModuleView, "FxModuleView");
    PIEJAM_GUI_MODEL(model::Info, "Info");
    PIEJAM_GUI_MODEL(model::IntParameter, "IntParameter");
    PIEJAM_GUI_MODEL(model::Log, "Log");
    PIEJAM_GUI_MODEL(model::MidiAssignable, "MidiAssignable");
    PIEJAM_GUI_MODEL(model::MidiInputSettings, "MidiInputSettings");
    PIEJAM_GUI_MODEL(model::Mixer, "Mixer");
    PIEJAM_GUI_MODEL(model::MixerChannel, "MixerChannel");
    PIEJAM_GUI_MODEL(model::MixerChannelAdd, "MixerChannelAdd");
    PIEJAM_GUI_MODEL(model::MixerChannelAuxSend, "MixerChannelAuxSend");
    PIEJAM_GUI_MODEL(model::MixerChannelEdit, "MixerChannelEdit");
    PIEJAM_GUI_MODEL(model::MixerChannelFx, "MixerChannelFx");
    PIEJAM_GUI_MODEL(model::MixerChannelModels, "MixerChannelModels");
    PIEJAM_GUI_MODEL(model::MixerChannelPerform, "MixerChannelPerform");
    PIEJAM_GUI_MODEL(model::Parameter, "Parameter");
    PIEJAM_GUI_MODEL(model::SessionSettings, "SessionSettings");
    PIEJAM_GUI_MODEL(model::Root, "Root");
    PIEJAM_GUI_MODEL(model::ScopeSlot, "ScopeSlot");
    PIEJAM_GUI_MODEL(model::SpectrumSlot, "SpectrumSlot");
    PIEJAM_GUI_MODEL(model::StereoLevel, "StereoLevel");
    PIEJAM_GUI_MODEL(model::String, "String");
    PIEJAM_GUI_MODEL(model::StringList, "StringList");
    PIEJAM_GUI_MODEL(model::UISettings, "UISettings");
    PIEJAM_GUI_MODEL(model::WaveformSlot, "WaveformSlot");

    qmlRegisterUncreatableMetaObject(
        model::staticMetaObject,
        "PieJam.Models",
        1,
        0,
        "Types",
        "C++ type");

    qmlRegisterSingletonInstance<model::MixerDbScales>(
        "PieJam.Models",
        1,
        0,
        "MixerDbScales",
        &model::g_mixerDbScales);

    qmlRegisterSingletonInstance<model::FxModuleRegistry>(
        "PieJam.Models",
        1,
        0,
        "FxModuleRegistry",
        &model::fxModuleRegistrySingleton());

    qmlRegisterSingletonInstance<model::Filesystem>(
        "PieJam.Models",
        1,
        0,
        "Filesystem",
        &model::filesystemSingleton());

    qRegisterMetaType<model::FilePath>();

    PIEJAM_GUI_ITEM(item::FixedLinearScaleGrid, "FixedLinearScaleGrid");
    PIEJAM_GUI_ITEM(item::FixedLogScaleGrid, "FixedLogScaleGrid");
    PIEJAM_GUI_ITEM(item::DbScale, "DbScale");
    PIEJAM_GUI_ITEM(item::Scope, "Scope");
    PIEJAM_GUI_ITEM(item::Spectrum, "Spectrum");
    PIEJAM_GUI_ITEM(item::Waveform, "Waveform");
}

#undef PIEJAM_GUI_MODEL
#undef PIEJAM_GUI_ITEM

} // namespace

void
init()
{
    std::once_flag s_run_once;
    std::call_once(s_run_once, []() {
        initResources();
        runRegistration();
    });
}

} // namespace piejam::gui
