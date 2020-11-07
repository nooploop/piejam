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

#include <piejam/audio/engine/graph.h>

#include <piejam/audio/engine/event_port.h>
#include <piejam/functional/partial_compare.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

void
graph::add_wire(endpoint const& src, endpoint const& dst)
{
    BOOST_ASSERT(src.port < src.proc.get().num_outputs());
    BOOST_ASSERT(dst.port < dst.proc.get().num_inputs());

    BOOST_ASSERT_MSG(
            std::ranges::none_of(
                    m_wires,
                    [&dst](auto const& wire) { return wire.second == dst; }),
            "destination endpoint already added, missing a mixer?");

    m_wires.emplace(src, dst);
}

void
graph::remove_wire(endpoint const& src, endpoint const& dst)
{
    auto from_source = m_wires.equal_range(src);
    auto it = std::ranges::find_if(
            from_source.first,
            from_source.second,
            equal_to(dst),
            &wires_t::value_type::second);
    BOOST_ASSERT(it != m_wires.end());
    m_wires.erase(it);
}

void
graph::add_event_wire(const endpoint& src, const endpoint& dst)
{
    BOOST_ASSERT(src.port < src.proc.get().event_outputs().size());
    BOOST_ASSERT(dst.port < dst.proc.get().event_inputs().size());
    BOOST_ASSERT(
            src.proc.get().event_outputs()[src.port].type() ==
            dst.proc.get().event_inputs()[dst.port].type());

    BOOST_ASSERT_MSG(
            std::ranges::none_of(
                    m_event_wires,
                    [&dst](auto const& wire) { return wire.second == dst; }),
            "destination endpoint already added, missing an event mixer?");

    m_event_wires.emplace(src, dst);
}

} // namespace piejam::audio::engine
