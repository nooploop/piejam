// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/modules/factory.h>
#include <piejam/runtime/parameters.h>

#include <piejam/audio/types.h>

namespace piejam::fx_modules::filter
{

enum class type
{
    bypass,
    lp2,
    lp4,
    bp2,
    bp4,
    hp2,
    hp4,
    br
};

enum class parameter_key : runtime::fx::parameter_key
{
    type,
    cutoff,
    resonance
};

enum class stream_key : runtime::fx::stream_key
{
    in_out
};

auto make_module(runtime::modules::internal_fx_module_factory_args const&)
        -> runtime::fx::module;

} // namespace piejam::fx_modules::filter
