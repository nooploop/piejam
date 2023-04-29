// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/processor.h>

#include <memory>
#include <utility>
#include <vector>

namespace piejam::audio::engine
{

namespace endpoint_indices
{

template <std::size_t... Is>
using from = std::index_sequence<Is...>;

template <std::size_t... Is>
using to = std::index_sequence<Is...>;

static inline constexpr std::index_sequence<0, 1> stereo;

} // namespace endpoint_indices

void
connect(graph&,
        graph_endpoint const& src,
        graph_endpoint const& dst,
        std::vector<std::unique_ptr<processor>>& mixers);

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        processor& src,
        endpoint_indices::from<M...>,
        processor& dst,
        endpoint_indices::to<N...>,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (connect(g,
             graph_endpoint{.proc = src, .port = M},
             graph_endpoint{.proc = dst, .port = N},
             mixers),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        processor& src,
        endpoint_indices::from<M...>,
        processor& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.audio.insert(
             graph_endpoint{.proc = src, .port = M},
             graph_endpoint{.proc = dst, .port = N}),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        processor& src,
        endpoint_indices::from<M...>,
        component const& dst,
        endpoint_indices::to<N...>,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (connect(g,
             graph_endpoint{.proc = src, .port = M},
             dst.inputs()[N],
             mixers),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        processor& src,
        endpoint_indices::from<M...>,
        component const& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.audio.insert(graph_endpoint{.proc = src, .port = M}, dst.inputs()[N]),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        component const& src,
        endpoint_indices::from<M...>,
        processor& dst,
        endpoint_indices::to<N...>,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (connect(g,
             src.outputs()[M],
             graph_endpoint{.proc = dst, .port = N},
             mixers),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        component const& src,
        endpoint_indices::from<M...>,
        processor& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.audio.insert(src.outputs()[M], graph_endpoint{.proc = dst, .port = N}),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        component const& src,
        endpoint_indices::from<M...>,
        component const& dst,
        endpoint_indices::to<N...>,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (connect(g, src.outputs()[M], dst.inputs()[N], mixers), ...);
}

template <std::size_t... M, std::size_t... N>
void
connect(graph& g,
        component const& src,
        endpoint_indices::from<M...>,
        component const& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.audio.insert(src.outputs()[M], dst.inputs()[N]), ...);
}

template <std::size_t... M, std::size_t... N>
void
connect_event(
        graph& g,
        processor& src,
        endpoint_indices::from<M...>,
        processor& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.event.insert(
             graph_endpoint{.proc = src, .port = M},
             graph_endpoint{.proc = dst, .port = N}),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect_event(
        graph& g,
        processor& src,
        endpoint_indices::from<M...>,
        component const& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.event.insert(
             graph_endpoint{.proc = src, .port = M},
             dst.event_inputs()[N]),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect_event(
        graph& g,
        component const& src,
        endpoint_indices::from<M...>,
        processor& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.event.insert(
             src.event_outputs()[M],
             graph_endpoint{.proc = dst, .port = N}),
     ...);
}

template <std::size_t... M, std::size_t... N>
void
connect_event(
        graph& g,
        component const& src,
        endpoint_indices::from<M...>,
        component const& dst,
        endpoint_indices::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.event.insert(src.event_outputs()[M], dst.event_inputs()[N]), ...);
}

} // namespace piejam::audio::engine
