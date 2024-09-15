// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/fwd.h>

#include <vector>

namespace piejam::midi
{

using device_id_t = entity_id<struct device_id_tag>;
using device_ids_t = std::vector<device_id_t>;

} // namespace piejam::midi
