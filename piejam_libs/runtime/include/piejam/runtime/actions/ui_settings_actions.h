// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct set_display_rotation final
    : ui::cloneable_action<set_display_rotation, reducible_action>
{
    std::size_t display_rotation{};

    void reduce(state&) const override;
};

struct set_on_screen_keyboard_enabled final
    : ui::cloneable_action<set_display_rotation, reducible_action>
{
    bool enabled{};

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
