// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/actions/ladspa_fx_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct delete_fx_module final
    : ui::cloneable_action<delete_fx_module, reducible_action>
    , visitable_engine_action<delete_fx_module>
    , visitable_ladspa_fx_action<delete_fx_module>
{
    mixer::channel_id fx_chain_id;
    fx::module_id fx_mod_id;

    [[nodiscard]] auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
