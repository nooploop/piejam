// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/processor.h>

#include <boost/assert.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace piejam::audio::engine
{

namespace endpoint_ports
{

template <std::size_t... Is>
using from = std::index_sequence<Is...>;

template <std::size_t... Is>
using to = std::index_sequence<Is...>;

static inline constexpr std::index_sequence<0, 1> stereo;

} // namespace endpoint_ports

void
connect(graph&,
        graph_endpoint const& src,
        graph_endpoint const& dst,
        std::vector<std::unique_ptr<processor>>& mixers);

namespace detail
{

template <class C>
concept graph_entity = std::is_base_of_v<component, std::remove_cvref_t<C>> ||
                       std::is_base_of_v<processor, std::remove_cvref_t<C>>;

inline constexpr auto
src_endpoint(component const& src, std::size_t port) noexcept
{
    return src.outputs()[port];
}

inline constexpr auto
src_endpoint(processor& src, std::size_t port) noexcept
{
    return graph_endpoint{.proc = src, .port = port};
}

inline constexpr auto
dst_endpoint(component const& src, std::size_t port) noexcept
{
    return src.inputs()[port];
}

inline constexpr auto
dst_endpoint(processor& src, std::size_t port) noexcept
{
    return graph_endpoint{.proc = src, .port = port};
}

inline constexpr auto
src_event_endpoint(component const& src, std::size_t port) noexcept
{
    return src.event_outputs()[port];
}

inline constexpr auto
src_event_endpoint(processor& src, std::size_t port) noexcept
{
    return graph_endpoint{.proc = src, .port = port};
}

inline constexpr auto
dst_event_endpoint(component const& src, std::size_t port) noexcept
{
    return src.event_inputs()[port];
}

inline constexpr auto
dst_event_endpoint(processor& src, std::size_t port) noexcept
{
    return graph_endpoint{.proc = src, .port = port};
}

} // namespace detail

template <
        detail::graph_entity Src,
        std::size_t... M,
        detail::graph_entity Dst,
        std::size_t... N>
void
connect(graph& g,
        Src&& src,
        endpoint_ports::from<M...>,
        Dst&& dst,
        endpoint_ports::to<N...>)
{
    static_assert(sizeof...(M) == sizeof...(N));

    (g.audio.insert(
             detail::src_endpoint(std::forward<Src>(src), M),
             detail::dst_endpoint(std::forward<Dst>(dst), N)),
     ...);
}

template <detail::graph_entity Src, detail::graph_entity Dst>
void
connect(graph& g, Src&& src, Dst&& dst)
{
    BOOST_ASSERT(src.num_outputs() == dst.num_inputs());

    for (std::size_t p = 0, e = src.num_outputs(); p < e; ++p)
    {
        g.audio.insert(
                detail::src_endpoint(std::forward<Src>(src), p),
                detail::dst_endpoint(std::forward<Dst>(dst), p));
    }
}

template <
        detail::graph_entity Src,
        std::size_t... M,
        detail::graph_entity Dst,
        std::size_t... N>
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
             detail::src_endpoint(std::forward<Src>(src), M),
             detail::dst_endpoint(std::forward<Dst>(dst), N),
             mixers),
     ...);
}

template <detail::graph_entity Src, detail::graph_entity Dst>
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
                detail::src_endpoint(std::forward<Src>(src), p),
                detail::dst_endpoint(std::forward<Dst>(dst), p),
                mixers);
    }
}

template <
        detail::graph_entity Src,
        std::size_t... M,
        detail::graph_entity Dst,
        std::size_t... N>
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
             detail::src_event_endpoint(std::forward<Src>(src), M),
             detail::dst_event_endpoint(std::forward<Dst>(dst), N)),
     ...);
}

} // namespace piejam::audio::engine
