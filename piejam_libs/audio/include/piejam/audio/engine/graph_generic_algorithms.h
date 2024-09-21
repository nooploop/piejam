// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/endpoint_ports.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_node.h>

#include <piejam/range/indices.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace piejam::audio::engine
{

template <graph_node Src, std::size_t... M, graph_node Dst, std::size_t... N>
void
connect(graph& g,
        Src&& src,
        endpoint_ports::from_t<M...>,
        Dst&& dst,
        endpoint_ports::to_t<N...>)
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
    for (std::size_t p = 0, e = std::min(src.num_outputs(), dst.num_inputs());
         p < e;
         ++p)
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
        endpoint_ports::from_t<M...>,
        Dst&& dst,
        endpoint_ports::to_t<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.event.insert(
             src_event_endpoint(std::forward<Src>(src), M),
             dst_event_endpoint(std::forward<Dst>(dst), N)),
     ...);
}

template <graph_node Dst, graph_node... Src>
void
connect_event_to(graph& g, Dst&& dst, Src&&... src)
{
    auto connect_single =
            [&]<class SingleSrc>(SingleSrc&& s, std::size_t& dst_port) {
                for (std::size_t src_port : range::indices(s.event_outputs()))
                {
                    g.event.insert(
                            src_event_endpoint(s, src_port),
                            dst_event_endpoint(dst, dst_port++));
                }
            };

    std::size_t dst_port{};
    (connect_single(src, dst_port), ...);
}

template <graph_node... Node>
auto
in_event_endpoints(Node&&... node)
{
    std::vector<graph_endpoint> result;

    (std::ranges::transform(
             range::indices(node.event_inputs()),
             std::back_inserter(result),
             std::bind_front(
                     &in_event_endpoint<Node>,
                     std::ref(std::forward<Node>(node)))),
     ...);

    return result;
}

template <graph_node... Node>
auto
out_event_endpoints(Node&&... node)
{
    std::vector<graph_endpoint> result;

    (std::ranges::transform(
             range::indices(node.event_outputs()),
             std::back_inserter(result),
             std::bind_front(
                     &out_event_endpoint<Node>,
                     std::ref(std::forward<Node>(node)))),
     ...);

    return result;
}

} // namespace piejam::audio::engine
