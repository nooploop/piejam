// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/parameters.h>

namespace piejam::runtime::fx
{

enum class gain_parameter_key : parameter_key
{
    gain
};

auto make_gain_module(parameters_t&, float_parameters&) -> module;

} // namespace piejam::runtime::fx
