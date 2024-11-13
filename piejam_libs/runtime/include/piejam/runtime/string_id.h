// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/fwd.h>

#include <string>

namespace piejam::runtime
{

using string_id = entity_id<struct string_id_tag>;

using strings_t = entity_data_map<string_id, box<std::string>>;

} // namespace piejam::runtime
