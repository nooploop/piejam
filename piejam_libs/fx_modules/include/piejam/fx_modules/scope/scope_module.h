// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/internal_fx_module_factory.h>

#include <piejam/audio/types.h>

namespace piejam::fx_modules::scope
{

enum class mode : int
{
    free,
    trigger_a,
    trigger_b,

    trigger = trigger_a, // for mono

    _min = free,
};

enum class trigger_slope : int
{
    rising_edge,
    falling_edge,

    _min = rising_edge,
    _max = falling_edge,
};

enum class window_size : int
{
    very_small,
    small,
    medium,
    large,
    very_large,

    _min = very_small,
    _max = very_large,
};

enum class stereo_channel : int
{
    left,
    right,
    middle,
    side,

    _min = left,
    _max = side,
};

enum class parameter_key : runtime::fx::parameter_key
{
    mode,
    trigger_slope,
    trigger_level,
    hold_time,
    waveform_window_size,
    scope_window_size,
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

} // namespace piejam::fx_modules::scope
