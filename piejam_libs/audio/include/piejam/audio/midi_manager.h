// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/midi_device_id.h>
#include <piejam/entity_id.h>

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace piejam::audio
{

struct midi_device_added
{
    midi_device_id_t device_id;
    std::string name;
};

struct midi_device_removed
{
    midi_device_id_t device_id;
};

using midi_device_update =
        std::variant<std::nullptr_t, midi_device_added, midi_device_removed>;

class midi_manager
{
public:
    midi_manager();
    ~midi_manager();

    bool enable_input_device(midi_device_id_t const&);
    void disable_input_device(midi_device_id_t const&);

    auto update_devices() -> std::vector<midi_device_update>;

private:
    struct impl;
    std::unique_ptr<impl> const m_impl;
};

} // namespace piejam::audio
