// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::gui::model
{

template <class ListItem>
class GenericListModel;

class EnumListModel;
class StringList;

class String;
using Strings = GenericListModel<String>;

using MaterialColor = int;

class AudioDeviceSettings;
class AudioInputOutputSettings;

class FxChainModule;
using FxChainModulesList = GenericListModel<FxChainModule>;
class FxModuleView;
class FxModule;

class Info;
class Log;

class Mixer;
class MixerChannelModels;
using MixerChannelsList = GenericListModel<MixerChannelModels>;
class MixerChannelPerform;
class MixerChannelEdit;
class MixerChannelFx;
class MixerChannelAuxSend;
class DbScaleData;
class AudioRouting;
class AudioRoutingSelection;

class ExternalAudioDeviceConfig;
using ExternalAudioDeviceConfigList =
        GenericListModel<ExternalAudioDeviceConfig>;

class Parameter;
using FxParametersList = GenericListModel<Parameter>;
class BoolParameter;
class IntParameter;
class EnumParameter;
class FloatParameter;
class StereoLevel;

class FxBrowser;
class FxBrowserEntry;
using FxBrowserList = GenericListModel<FxBrowserEntry>;

class AudioStreamProvider;
using AudioStreamProviderList = GenericListModel<AudioStreamProvider>;

class WaveformSlot;
class Waveform;
class WaveformGenerator;

class SpectrumSlot;

class ScopeSlot;

class MidiDeviceConfig;
using MidiDeviceList = GenericListModel<MidiDeviceConfig>;

class MidiInputSettings;

class MidiAssignable;

class RootView;

} // namespace piejam::gui::model
