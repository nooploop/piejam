// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::runtime::modules::scope
{

enum class stream_key : fx::stream_key
{
    left_right
};

auto make_module(audio_streams_cache&) -> fx::module;

} // namespace piejam::runtime::modules::scope
