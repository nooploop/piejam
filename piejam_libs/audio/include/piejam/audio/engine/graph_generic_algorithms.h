// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/endpoint_ports.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_node.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/range/indices.h>

#include <boost/assert.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace piejam::audio::engine
{

template <graph_node Src, std::size_t... M, graph_node Dst, std::size_t... N>
void
connect(graph& g,
        Src&& src,
        endpoint_ports::from<M...>,
        Dst&& dst,
        endpoint_ports::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.audio.insert(
             src_endpoint(std::forward<Src>(src), M),
             dst_endpoint(std::forward<Dst>(dst), N)),
     ...);
}

template <graph_node Src, graph_node Dst>
void
connect(graph& g, Src&& src, Dst&& dst)
{
    BOOST_ASSERT(src.num_outputs() == dst.num_inputs());

    for (std::size_t p = 0, e = src.num_outputs(); p < e; ++p)
    {
        g.audio.insert(
                src_endpoint(std::forward<Src>(src), p),
                dst_endpoint(std::forward<Dst>(dst), p));
    }
}

template <graph_node Src, std::size_t... M, graph_node Dst, std::size_t... N>
void
connect_event(
        graph& g,
        Src&& src,
        endpoint_ports::from<M...>,
        Dst&& dst,
        endpoint_ports::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.event.insert(
             src_event_endpoint(std::forward<Src>(src), M),
             dst_event_endpoint(std::forward<Dst>(dst), N)),
     ...);
}

template <graph_node Node>
auto
in_event_endpoints(Node&& node)
{
    return algorithm::transform_to_vector(
            range::indices(node.event_inputs()),
            [&](std::size_t i) { return dst_event_endpoint(node, i); });
}

template <graph_node Node>
auto
out_event_endpoints(Node&& node)
{
    return algorithm::transform_to_vector(
            range::indices(node.event_outputs()),
            [&](std::size_t i) { return src_event_endpoint(node, i); });
}

} // namespace piejam::audio::engine
