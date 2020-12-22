// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/app/gui/model/Factory.h>

#include <piejam/app/gui/model/AudioDeviceSettings.h>
#include <piejam/app/gui/model/AudioInputOutputSettings.h>
#include <piejam/app/gui/model/FxBrowser.h>
#include <piejam/app/gui/model/FxChain.h>
#include <piejam/app/gui/model/Info.h>
#include <piejam/app/gui/model/Mixer.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/gui/model/MixerChannel.h>

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
    , m_mixer(std::make_unique<Mixer>(dispatch, state_change_subscriber))
    , m_info(std::make_unique<Info>(dispatch, state_change_subscriber))
    , m_fxChain(std::make_unique<FxChain>(dispatch, state_change_subscriber))
    , m_fxBrowser(
              std::make_unique<FxBrowser>(dispatch, state_change_subscriber))
{
    qRegisterMetaType<piejam::gui::model::AudioDeviceSettings*>();
    qRegisterMetaType<piejam::gui::model::AudioInputOutputSettings*>();
    qRegisterMetaType<piejam::gui::model::Mixer*>();
    qRegisterMetaType<piejam::gui::model::Info*>();
    qRegisterMetaType<piejam::gui::model::FxChain*>();
    qRegisterMetaType<piejam::gui::model::FxBrowser*>();
}

Factory::~Factory() = default;

} // namespace piejam::app::gui::model
