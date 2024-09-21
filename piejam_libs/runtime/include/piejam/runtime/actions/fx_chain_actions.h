// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/audio_engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct toggle_focused_fx_module_bypass final
    : ui::cloneable_action<toggle_focused_fx_module_bypass, reducible_action>
    , visitable_audio_engine_action<toggle_focused_fx_module_bypass>
{
    void reduce(state&) const override;
};

struct focus_fx_module final
    : ui::cloneable_action<focus_fx_module, reducible_action>
{
    mixer::channel_id fx_chain_id;
    fx::module_id fx_mod_id;

    void reduce(state&) const override;
};

struct show_fx_module final
    : ui::cloneable_action<show_fx_module, reducible_action>
{
    mixer::channel_id fx_chain_id;
    fx::module_id fx_mod_id;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
