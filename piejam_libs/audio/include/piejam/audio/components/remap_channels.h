// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/graph_node.h>

#include <boost/assert.hpp>

#include <array>
#include <memory>
#include <utility>

namespace piejam::audio::components
{

template <engine::graph_node Node, class Ins, class Outs>
class remap_channels;

template <engine::graph_node Node, std::size_t... I, std::size_t... O>
class remap_channels<Node, std::index_sequence<I...>, std::index_sequence<O...>>
        final : public engine::component
{
public:
    remap_channels(
            std::unique_ptr<Node> node,
            std::index_sequence<I...>,
            std::index_sequence<O...>)
        : m_node{std::move(node)}
    {
        BOOST_ASSERT(m_node);
    }

    [[nodiscard]] auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    [[nodiscard]] auto outputs() const -> endpoints override
    {
        return m_outputs;
    }

    [[nodiscard]] auto event_inputs() const -> endpoints override
    {
        return m_node->event_inputs();
    }

    [[nodiscard]] auto event_outputs() const -> endpoints override
    {
        return m_node->event_outputs();
    }

    void connect(engine::graph& g) const override
    {
        m_node->connect(g);
    }

private:
    std::unique_ptr<Node> m_node;

    std::array<engine::graph_endpoint, sizeof...(I)> m_inputs{
            engine::dst_endpoint(*m_node, I)...};
    std::array<engine::graph_endpoint, sizeof...(O)> m_outputs{
            engine::src_endpoint(*m_node, O)...};
};

template <engine::graph_node Node, std::size_t... I, std::size_t... O>
auto
make_remap_channels(
        std::unique_ptr<Node> node,
        std::index_sequence<I...> ins,
        std::index_sequence<O...> outs) -> std::unique_ptr<engine::component>
{
    return std::make_unique<remap_channels<
            Node,
            std::index_sequence<I...>,
            std::index_sequence<O...>>>(std::move(node), ins, outs);
}

} // namespace piejam::audio::components
