// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <optional>
#include <vector>

namespace piejam::audio::engine
{

auto connected_source(graph const&, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>;
auto connected_event_source(graph const&, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>;

auto has_audio_wire(
        graph const&,
        graph_endpoint const& src,
        graph_endpoint const& dst) -> bool;

auto has_event_wire(
        graph const&,
        graph_endpoint const& src,
        graph_endpoint const& dst) -> bool;

void remove_event_identity_processors(graph&);
void remove_identity_processors(graph&);

using mix_processors = std::vector<std::unique_ptr<processor>>;

auto finalize_graph(graph const&) -> std::tuple<graph, mix_processors>;

} // namespace piejam::audio::engine
