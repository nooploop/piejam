// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/midi/device_id.h>
#include <piejam/midi/fwd.h>

#include <memory>
#include <vector>

namespace piejam::midi
{

class device_manager
{
public:
    virtual ~device_manager() = default;

    virtual auto activate_input_device(device_id_t const&) -> bool = 0;
    virtual void deactivate_input_device(device_id_t const&) = 0;

    virtual auto update_devices() -> std::vector<device_update> = 0;

    virtual auto
    make_input_event_handler() -> std::unique_ptr<input_event_handler> = 0;
};

auto make_device_manager() -> std::unique_ptr<device_manager>;

} // namespace piejam::midi
