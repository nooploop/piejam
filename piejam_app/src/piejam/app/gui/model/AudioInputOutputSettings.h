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

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/types.h>
#include <piejam/gui/model/AudioInputOutputSettings.h>
#include <piejam/io_direction.h>

namespace piejam::app::gui::model
{

class AudioInputOutputSettings
    : public Subscribable<piejam::gui::model::AudioInputOutputSettings>
{
protected:
    AudioInputOutputSettings(
            runtime::store_dispatch,
            runtime::subscriber&,
            io_direction);

public:
    void addMonoBus() override;
    void addStereoBus() override;

private:
    void onSubscribe() override;

    void addBus(audio::bus_type);

    io_direction m_settings_type;

    runtime::mixer::bus_list_t m_bus_ids;
};

class AudioInputSettings final : public AudioInputOutputSettings
{
public:
    AudioInputSettings(runtime::store_dispatch st, runtime::subscriber& subs)
        : AudioInputOutputSettings(st, subs, io_direction::input)
    {
    }
};

class AudioOutputSettings final : public AudioInputOutputSettings
{
public:
    AudioOutputSettings(runtime::store_dispatch st, runtime::subscriber& subs)
        : AudioInputOutputSettings(st, subs, io_direction::output)
    {
    }
};

} // namespace piejam::app::gui::model
