// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/ladspa/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/parameters.h>

#include <span>
#include <string>

namespace piejam::runtime::modules::ladspa
{

auto make_module(
        fx::ladspa_instance_id,
        std::string const& name,
        std::span<piejam::ladspa::port_descriptor const> const& control_inputs,
        fx::parameters_t&,
        parameter_maps&) -> fx::module;

} // namespace piejam::runtime::modules::ladspa
