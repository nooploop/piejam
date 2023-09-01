// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/ladspa/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/parameters.h>

#include <span>
#include <string>

namespace piejam::runtime::modules::ladspa_fx
{

auto make_module(
        ladspa::instance_id,
        std::string const& name,
        audio::bus_type,
        std::span<piejam::ladspa::port_descriptor const> control_inputs,
        ui_parameter_factory const&) -> fx::module;

} // namespace piejam::runtime::modules::ladspa_fx
