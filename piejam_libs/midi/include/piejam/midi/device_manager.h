// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/midi/device_id.h>
#include <piejam/midi/fwd.h>

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace piejam::midi
{

struct device_added
{
    device_id_t device_id;
    std::string name;
};

struct device_removed
{
    device_id_t device_id;
};

using device_update = std::variant<device_added, device_removed>;

class device_manager
{
public:
    device_manager();
    ~device_manager();

    bool activate_input_device(device_id_t const&);
    void deactivate_input_device(device_id_t const&);

    auto update_devices() -> std::vector<device_update>;

    auto make_input_processor() const -> std::unique_ptr<input_processor>;

private:
    struct impl;
    std::unique_ptr<impl> const m_impl;
};

} // namespace piejam::midi
