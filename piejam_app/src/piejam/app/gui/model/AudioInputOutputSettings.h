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

#pragma once

#include <piejam/app/store.h>
#include <piejam/app/subscriber.h>
#include <piejam/audio/types.h>
#include <piejam/gui/model/AudioInputOutputSettings.h>
#include <piejam/reselect/subscriptions_manager.h>

namespace piejam::app::gui::model
{

class AudioInputOutputSettings
    : public piejam::gui::model::AudioInputOutputSettings
{
protected:
    AudioInputOutputSettings(store&, subscriber&, audio::bus_direction);

public:
    void setBusName(unsigned bus, QString const& name) override;
    void selectMonoChannel(unsigned bus, unsigned ch) override;
    void selectStereoLeftChannel(unsigned bus, unsigned ch) override;
    void selectStereoRightChannel(unsigned bus, unsigned ch) override;
    void addMonoBus() override;
    void addStereoBus() override;
    void deleteBus(unsigned bus) override;

private:
    void selectChannel(audio::bus_channel, unsigned bus, unsigned ch);
    void addBus(audio::bus_type);

    store& m_store;
    subscriptions_manager m_subs;
    audio::bus_direction m_settings_type;
};

class AudioInputSettings final : public AudioInputOutputSettings
{
public:
    AudioInputSettings(store& st, subscriber& subs)
        : AudioInputOutputSettings(st, subs, audio::bus_direction::input)
    {}
};

class AudioOutputSettings final : public AudioInputOutputSettings
{
public:
    AudioOutputSettings(store& st, subscriber& subs)
        : AudioInputOutputSettings(st, subs, audio::bus_direction::output)
    {}
};

} // namespace piejam::app::gui::model
