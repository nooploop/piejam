// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct add_bus final
    : ui::cloneable_action<add_bus, reducible_action>
    , visitable_engine_action<add_bus>
{
    io_direction direction{};
    audio::bus_type type{};

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
