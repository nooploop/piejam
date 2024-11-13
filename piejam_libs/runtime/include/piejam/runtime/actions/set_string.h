// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/string_id.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <piejam/boxed_string.h>
#include <piejam/entity_id.h>

namespace piejam::runtime::actions
{

struct set_string final : ui::cloneable_action<set_string, reducible_action>
{
    string_id id;
    boxed_string str;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
