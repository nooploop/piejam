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

#include <piejam/audio/engine/graph_algorithms.h>

#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/functional/partial_compare.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <ranges>

namespace piejam::audio::engine
{

namespace
{

auto
connected_source(graph::wires_t const& ws, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    auto it = std::ranges::find_if(
            ws,
            piejam::equal_to(dst),
            &graph::wires_t::value_type::second);
    if (it != ws.end())
        return it->first;
    else
        return {};
}

} // namespace

auto
connected_source(graph const& g, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    return connected_source(g.wires(), dst);
}

auto
connected_event_source(graph const& g, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    return connected_source(g.event_wires(), dst);
}

bool
has_wire(graph const& g, graph_endpoint const& src, graph_endpoint const& dst)
{
    auto connected = g.wires().equal_range(src);
    return connected.first != connected.second &&
           std::ranges::find(
                   connected.first,
                   connected.second,
                   dst,
                   &graph::wires_t::value_type::second) != connected.second;
}

auto
connect(graph& g,
        graph_endpoint const& src,
        graph_endpoint const& dst,
        std::vector<std::unique_ptr<processor>>& mixers)
        -> std::unique_ptr<processor>
{
    if (auto const connected_src = connected_source(g, dst))
    {
        if (is_mix_processor(connected_src->proc))
        {
            processor& prev_mixer = connected_src->proc;
            std::size_t const num_prev_inputs = prev_mixer.num_inputs();
            auto mixer = make_mix_processor(num_prev_inputs + 1);

            for (std::size_t in_index = 0; in_index < num_prev_inputs;
                 ++in_index)
            {
                graph_endpoint prev_mixer_in{prev_mixer, in_index};
                auto const connected_in_src =
                        connected_source(g, prev_mixer_in);
                BOOST_ASSERT(connected_in_src);
                g.remove_wire(*connected_in_src, prev_mixer_in);
                g.add_wire(*connected_in_src, {*mixer, in_index});
            }

            g.add_wire(src, {*mixer, num_prev_inputs});

            g.remove_wire({prev_mixer, 0}, dst);
            g.add_wire({*mixer, 0}, dst);

            auto it_mixer = std::ranges::find_if(
                    mixers,
                    [pma = std::addressof(prev_mixer)](auto const& m) {
                        return m.get() == pma;
                    });
            BOOST_ASSERT(it_mixer != mixers.end());
            std::swap(*it_mixer, mixer);
            return mixer;
        }
        else
        {
            g.remove_wire(*connected_src, dst);

            auto mixer = make_mix_processor(2);
            g.add_wire(*connected_src, {*mixer, 0});
            g.add_wire(src, {*mixer, 1});
            g.add_wire({*mixer, 0}, dst);
            mixers.emplace_back(std::move(mixer));
            return nullptr;
        }
    }
    else
    {
        g.add_wire(src, dst);
        return nullptr;
    }
}

} // namespace piejam::audio::engine
