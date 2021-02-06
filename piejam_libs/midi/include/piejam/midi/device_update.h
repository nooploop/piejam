// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/midi/device_id.h>

#include <string>
#include <variant>

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

} // namespace piejam::midi
