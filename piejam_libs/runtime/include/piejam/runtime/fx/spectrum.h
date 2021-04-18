// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::runtime::fx
{

enum class spectrum_stream_key : stream_key
{
    left_right
};

auto make_spectrum_module(audio_streams_cache&) -> module;

} // namespace piejam::runtime::fx
