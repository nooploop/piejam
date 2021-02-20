// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
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

struct add_mixer_bus final
    : ui::cloneable_action<add_mixer_bus, action>
    , visitable_engine_action<add_mixer_bus>
{
    auto reduce(state const&) const -> state override;
};

struct delete_mixer_bus final
    : ui::cloneable_action<delete_mixer_bus, action>
    , visitable_engine_action<delete_mixer_bus>
{
    mixer::bus_id bus_id{};

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
