// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/types.h>
#include <piejam/gui/model/AudioInputOutputSettings.h>
#include <piejam/io_direction.h>
#include <piejam/runtime/device_io_fwd.h>

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

    runtime::device_io::bus_list_t m_bus_ids;
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
