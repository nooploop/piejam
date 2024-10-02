// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/graph_endpoint.h>

#include <utility>

namespace piejam::audio::engine::endpoint_ports
{

template <std::size_t... Is>
struct from_t : std::index_sequence<Is...>
{
};

template <std::size_t... Is>
constexpr auto from = from_t<Is...>{};

template <std::size_t... Is>
struct to_t : std::index_sequence<Is...>
{
};

template <std::size_t... Is>
constexpr auto to = to_t<Is...>{};

} // namespace piejam::audio::engine::endpoint_ports
