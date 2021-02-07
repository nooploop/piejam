// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct delete_bus final
    : ui::cloneable_action<delete_bus, action>
    , visitable_engine_action<delete_bus>
{
    mixer::bus_id bus_id{};

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
