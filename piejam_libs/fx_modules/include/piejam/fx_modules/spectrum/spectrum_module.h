// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/internal_fx_module_factory.h>

#include <piejam/audio/types.h>

namespace piejam::fx_modules::spectrum
{

enum class stereo_channel : int
{
    left,
    right,
    middle,
    side,

    _min = left,
    _max = side,
};

enum class parameter_key : runtime::parameter::key
{
    stream_a_active,
    stream_b_active,
    channel_a,
    channel_b,
    gain_a,
    gain_b,
    freeze,
};

enum class stream_key : runtime::fx::stream_key
{
    input
};

auto make_module(runtime::internal_fx_module_factory_args const&)
        -> runtime::fx::module;

} // namespace piejam::fx_modules::spectrum
