// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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
class FxModule;
class FxModuleContent;

class Info;
class Log;

class Mixer;
class MixerChannel;
using MixerChannelsList = GenericListModel<MixerChannel>;
class MixerChannelPerform;
class MixerChannelEdit;
class MixerChannelFx;
class DbScaleData;

class BusConfig;
using BusConfigsList = GenericListModel<BusConfig>;

class BusName;
using BusNamesList = GenericListModel<BusName>;

class FxParameter;
using FxParametersList = GenericListModel<FxParameter>;
class FxBoolParameter;
class FxIntParameter;
class FxEnumParameter;
class FxFloatParameter;

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
