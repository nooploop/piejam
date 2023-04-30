// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/endpoint_ports.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_node.h>

#include <boost/assert.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace piejam::audio::engine
{

void
connect(graph&,
        graph_endpoint const& src,
        graph_endpoint const& dst,
        std::vector<std::unique_ptr<processor>>& mixers);

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
connect(graph& g,
        Src&& src,
        endpoint_ports::from<M...>,
        Dst&& dst,
        endpoint_ports::to<N...>,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (connect(g,
             src_endpoint(std::forward<Src>(src), M),
             dst_endpoint(std::forward<Dst>(dst), N),
             mixers),
     ...);
}

template <graph_node Src, graph_node Dst>
void
connect(graph& g,
        Src&& src,
        Dst&& dst,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    BOOST_ASSERT(src.outputs().size() == dst.inputs().size());

    for (std::size_t p = 0, e = src.outputs().size(); p < e; ++p)
    {
        connect(g,
                src_endpoint(std::forward<Src>(src), p),
                dst_endpoint(std::forward<Dst>(dst), p),
                mixers);
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

} // namespace piejam::audio::engine
