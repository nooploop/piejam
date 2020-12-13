// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

void bypass_event_identity_processors(graph&);

} // namespace piejam::audio::engine
