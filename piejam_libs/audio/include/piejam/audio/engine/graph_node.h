// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/processor.h>

#include <boost/assert.hpp>

#include <type_traits>

namespace piejam::audio::engine
{

template <class C>
concept graph_node = std::is_base_of_v<component, std::remove_cvref_t<C>> ||
                     std::is_base_of_v<processor, std::remove_cvref_t<C>>;

constexpr auto
src_endpoint(component const& src, std::size_t port) noexcept
{
    BOOST_ASSERT(port < src.num_outputs());
    return src.outputs()[port];
}

constexpr auto
src_endpoint(processor& src, std::size_t port) noexcept
{
    BOOST_ASSERT(port < src.num_outputs());
    return graph_endpoint{.proc = src, .port = port};
}

constexpr auto
dst_endpoint(component const& dst, std::size_t port) noexcept
{
    BOOST_ASSERT(port < dst.num_inputs());
    return dst.inputs()[port];
}

constexpr auto
dst_endpoint(processor& dst, std::size_t port) noexcept
{
    BOOST_ASSERT(port < dst.num_inputs());
    return graph_endpoint{.proc = dst, .port = port};
}

constexpr auto
src_event_endpoint(component const& src, std::size_t port) noexcept
{
    BOOST_ASSERT(port < src.event_outputs().size());
    return src.event_outputs()[port];
}

constexpr auto
src_event_endpoint(processor& src, std::size_t port) noexcept
{
    BOOST_ASSERT(port < src.event_outputs().size());
    return graph_endpoint{.proc = src, .port = port};
}

constexpr auto
dst_event_endpoint(component const& dst, std::size_t port) noexcept
{
    BOOST_ASSERT(port < dst.event_inputs().size());
    return dst.event_inputs()[port];
}

constexpr auto
dst_event_endpoint(processor& dst, std::size_t port) noexcept
{
    BOOST_ASSERT(port < dst.event_inputs().size());
    return graph_endpoint{.proc = dst, .port = port};
}

template <graph_node N>
constexpr auto
in_endpoint(N&& node, std::size_t port) noexcept
{
    return dst_endpoint(std::forward<N>(node), port);
}

template <graph_node N>
constexpr auto
out_endpoint(N&& node, std::size_t port) noexcept
{
    return src_endpoint(std::forward<N>(node), port);
}

template <graph_node N>
constexpr auto
in_event_endpoint(N&& node, std::size_t port) noexcept
{
    return dst_event_endpoint(std::forward<N>(node), port);
}

template <graph_node N>
constexpr auto
out_event_endpoint(N&& node, std::size_t port) noexcept
{
    return src_event_endpoint(std::forward<N>(node), port);
}

} // namespace piejam::audio::engine
