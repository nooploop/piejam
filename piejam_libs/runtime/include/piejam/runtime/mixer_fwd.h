// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>
#include <piejam/fwd.h>
#include <piejam/runtime/device_io_fwd.h>

#include <variant>
#include <vector>

namespace piejam::runtime::mixer
{

struct bus;
using bus_id = entity_id<bus>;
using buses_t = entity_map<bus>;

using missing_device_address = boxed_string;

using io_address_t = std::variant<
        std::nullptr_t,
        device_io::bus_id,
        missing_device_address,
        bus_id>;

using bus_io_t = io_pair<io_address_t>;

using bus_list_t = std::vector<bus_id>;

struct state;

} // namespace piejam::runtime::mixer
