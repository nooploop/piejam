// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/internal_fx_module_factory.h>

namespace piejam::fx_modules::dual_pan
{

enum class parameter_key : runtime::fx::parameter_key
{
    left_pan,
    right_pan
};

auto make_module(runtime::internal_fx_module_factory_args const&)
        -> runtime::fx::module;

} // namespace piejam::fx_modules::dual_pan
