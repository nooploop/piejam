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

#include <piejam/audio/engine/graph_endpoint.h>

#include <map>
#include <string>

namespace piejam::audio::engine
{

class graph
{
public:
    using wires_t = std::multimap<graph_endpoint, graph_endpoint>;

    auto wires() const noexcept -> wires_t const& { return m_wires; }
    void add_wire(graph_endpoint const& src, graph_endpoint const& dst);
    void remove_wire(graph_endpoint const& src, graph_endpoint const& dst);

    auto event_wires() const noexcept -> wires_t const&
    {
        return m_event_wires;
    }
    void add_event_wire(graph_endpoint const& src, graph_endpoint const& dst);

private:
    wires_t m_wires;
    wires_t m_event_wires;
};

auto export_graph_as_dot(graph const&) -> std::string;

} // namespace piejam::audio::engine
