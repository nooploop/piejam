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

class AudioDeviceSettings;
class AudioInputOutputSettings;

class FxChainModule;
using FxChainModulesList = GenericListModel<FxChainModule>;
class FxModuleView;
class FxModuleContent;

class Info;
class Log;

class Mixer;
class MixerChannel;
using MixerChannelsList = GenericListModel<MixerChannel>;
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
class StereoLevelParameter;

class FxBrowser;
class FxBrowserEntry;
using FxBrowserList = GenericListModel<FxBrowserEntry>;

class AudioStreamListener;
class AudioStreamProvider;
using AudioStreamProviderList = GenericListModel<AudioStreamProvider>;

class WaveformDataObject;
class WaveformData;
class WaveformDataGenerator;

class SpectrumData;

class ScopeData;

class MidiDeviceConfig;
using MidiDeviceList = GenericListModel<MidiDeviceConfig>;

class MidiInputSettings;

class MidiAssignable;

class RootView;

} // namespace piejam::gui::model
