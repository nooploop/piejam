// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/root_view_mode.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <string>

namespace piejam::runtime::actions
{

struct set_root_view_mode final
    : ui::cloneable_action<set_root_view_mode, reducible_action>
{
    root_view_mode mode;

    [[nodiscard]] auto reduce(state const&) const -> state override;
};

struct show_fx_browser final
    : ui::cloneable_action<show_fx_browser, reducible_action>
{
    mixer::channel_id fx_chain_id;

    [[nodiscard]] auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
