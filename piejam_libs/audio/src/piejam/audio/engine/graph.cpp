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

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

void
graph::add_wire(
        processor& src_proc,
        std::size_t src_port,
        processor& dst_proc,
        std::size_t dst_port)
{
    BOOST_ASSERT(src_port < src_proc.num_outputs());
    BOOST_ASSERT(dst_port < dst_proc.num_inputs());

    BOOST_ASSERT_MSG(
            m_wires.count({src_proc, src_port}) == 0,
            "source endpoint already added, missing a spreader?");

    BOOST_ASSERT_MSG(
            std::none_of(
                    m_wires.begin(),
                    m_wires.end(),
                    [dst = endpoint{dst_proc, dst_port}](auto const& wire) {
                        return wire.second == dst;
                    }),
            "destination endpoint already added, missing a mixer?");

    m_wires.emplace(endpoint{src_proc, src_port}, endpoint{dst_proc, dst_port});
}

} // namespace piejam::audio::engine
