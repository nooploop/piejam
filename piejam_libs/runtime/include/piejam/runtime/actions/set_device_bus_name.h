// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <string>

namespace piejam::runtime::actions
{

struct set_device_bus_name final
    : ui::cloneable_action<set_device_bus_name, action>
{
    device_io::bus_id bus_id;
    std::string name;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
