// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/fwd.h>

#include <vector>

namespace piejam::runtime::external_audio
{

struct device;
using device_id = entity_id<device>;
using devices_t = entity_map<device>;

using device_ids_t = std::vector<device_id>;

struct state;

} // namespace piejam::runtime::external_audio
