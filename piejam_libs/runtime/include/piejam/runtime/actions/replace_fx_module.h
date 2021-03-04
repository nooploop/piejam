// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <string_view>

namespace piejam::runtime::actions
{

auto make_replace_fx_module_action(
        state const&,
        mixer::channel_id fx_chain_bus,
        std::size_t position,
        fx::internal) -> batch_action;

auto replace_fx_module(
        mixer::channel_id fx_chain_bus,
        std::size_t position,
        fx::internal) -> thunk_action;
auto replace_fx_module(std::size_t position, fx::internal) -> thunk_action;

auto make_replace_fx_module_action(
        state const&,
        mixer::channel_id fx_chain_bus,
        std::size_t position,
        audio::ladspa::plugin_id_t,
        std::string_view const& name,
        std::vector<fx::parameter_value_assignment> const& initial_values)
        -> batch_action;

auto replace_fx_module(
        mixer::channel_id fx_chain_bus,
        std::size_t position,
        audio::ladspa::plugin_id_t,
        std::string_view const& name) -> thunk_action;
auto replace_fx_module(
        std::size_t position,
        audio::ladspa::plugin_id_t,
        std::string_view const& name) -> thunk_action;

} // namespace piejam::runtime::actions
