// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::gui::model
{

template <class ListItem>
class ObjectListModel;

class EnumListModel;
class StringList;

class String;
using Strings = ObjectListModel<String>;

using MaterialColor = int;

class AudioDeviceSettings;
class SoundCardInfo;
class AudioInputOutputSettings;

class FxChainModule;
using FxChainModulesList = ObjectListModel<FxChainModule>;
class FxModuleView;
class FxModule;

class Info;
class Log;

class Mixer;
class MixerChannelModels;
using MixerChannelsList = ObjectListModel<MixerChannelModels>;
class MixerChannelPerform;
class MixerChannelEdit;
class MixerChannelFx;
class MixerChannelAuxSend;
class MixerChannelAdd;
class DbScaleData;
class AudioRouting;
class AudioRoutingSelection;
class AuxSend;
using AuxSendsList = ObjectListModel<AuxSend>;
class AuxChannel;

class ExternalAudioDeviceConfig;
using ExternalAudioDeviceConfigList =
    ObjectListModel<ExternalAudioDeviceConfig>;

class Parameter;
using FxParametersList = ObjectListModel<Parameter>;
class BoolParameter;
class IntParameter;
class EnumParameter;
class EnumListModel;
class FloatParameter;
class StereoLevel;

class FxBrowser;
class FxBrowserEntry;
using FxBrowserList = ObjectListModel<FxBrowserEntry>;

class AudioStreamProvider;
using AudioStreamProviderList = ObjectListModel<AudioStreamProvider>;

class WaveformSlot;
class Waveform;
class WaveformGenerator;

class SpectrumSlot;

class ScopeSlot;

class MidiDeviceConfig;
using MidiDeviceList = ObjectListModel<MidiDeviceConfig>;
class MidiInputSettings;

class UISettings;
class SessionSettings;

class MidiAssignable;

class FileDialog;
class FileDialogEntry;

class RootView;

} // namespace piejam::gui::model
