// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/device_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/persistence/app_config.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct apply_app_config final
    : ui::cloneable_action<apply_app_config, action>
    , visitable_device_action<apply_app_config>
{
    persistence::app_config conf;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
