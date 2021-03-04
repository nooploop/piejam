// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct select_fx_chain_bus final
    : ui::cloneable_action<select_fx_chain_bus, action>
{
    mixer::channel_id channel_id{};

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
