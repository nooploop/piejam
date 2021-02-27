// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/parameters.h>

#include <span>
#include <string>

namespace piejam::runtime::fx
{

auto make_ladspa_module(
        ladspa_instance_id,
        std::string const& name,
        std::span<audio::ladspa::port_descriptor const> control_inputs,
        parameters_t&,
        parameter_maps&) -> module;

} // namespace piejam::runtime::fx
