// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/graph_endpoint.h>

#include <utility>

namespace piejam::audio::engine::endpoint_ports
{

template <std::size_t... Is>
using from = std::index_sequence<Is...>;

template <std::size_t... Is>
using to = std::index_sequence<Is...>;

static inline constexpr std::index_sequence<0> mono;
static inline constexpr std::index_sequence<0, 1> stereo;

} // namespace piejam::audio::engine
