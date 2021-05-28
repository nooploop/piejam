// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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

bool
has_wire(graph const&, graph_endpoint const& src, graph_endpoint const& dst);

bool has_event_wire(
        graph const&,
        graph_endpoint const& src,
        graph_endpoint const& dst);

//! Smart connect function, which will insert a mixer if connecting to an
//! already connected destination. The mixer will be stored in the mixers
//! vector.
void
connect(graph&,
        graph_endpoint const& src,
        graph_endpoint const& dst,
        std::vector<std::unique_ptr<processor>>& mixers);

void
connect_stereo_components(graph&, component const& src, component const& dst);

void connect_stereo_components(
        graph&,
        component const& src,
        component const& dst,
        std::vector<std::unique_ptr<processor>>& mixers);

void connect_stereo_components(graph&, component const& src, processor& dst);

void remove_event_identity_processors(graph&);
void remove_identity_processors(graph&);

} // namespace piejam::audio::engine
