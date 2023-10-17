// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/graph_node.h>
#include <piejam/audio/engine/identity_processor.h>

#include <boost/assert.hpp>

#include <array>
#include <memory>
#include <span>
#include <utility>

namespace piejam::audio::components
{

template <engine::graph_node Node, class... ChannelMappings>
class remap_input_channels final : public engine::component
{
public:
    remap_input_channels(std::unique_ptr<Node> node, ChannelMappings...)
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
        return m_node->outputs();
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

        connect_inputs(g, std::make_index_sequence<num_inputs>{});
    }

private:
    template <std::size_t I, std::size_t... Is>
    void connect_input(engine::graph& g, std::index_sequence<Is...>) const
    {
        (engine::connect(
                 g,
                 *m_input_procs[I],
                 engine::endpoint_ports::from<0>{},
                 *m_node,
                 engine::endpoint_ports::to<Is>{}),
         ...);
    }

    template <std::size_t... I>
    void connect_inputs(engine::graph& g, std::index_sequence<I...>) const
    {
        (connect_input<I>(g, ChannelMappings{}), ...);
    }

    std::unique_ptr<Node> m_node;

    constexpr static inline std::size_t const num_inputs{
            sizeof...(ChannelMappings)};

    using input_procs_t =
            std::array<std::unique_ptr<engine::processor>, num_inputs>;

    template <std::size_t... I>
    static auto make_input_procs(std::index_sequence<I...>) -> input_procs_t
    {
        return input_procs_t{((void)I, engine::make_identity_processor())...};
    }

    input_procs_t m_input_procs{
            make_input_procs(std::make_index_sequence<num_inputs>{})};

    using inputs_t = std::array<engine::graph_endpoint, num_inputs>;

    template <std::size_t... I>
    static auto make_inputs(
            std::span<std::unique_ptr<engine::processor> const> const
                    input_procs,
            std::index_sequence<I...>) -> inputs_t
    {
        return inputs_t{engine::dst_endpoint(*input_procs[I], 0)...};
    }

    inputs_t m_inputs{
            make_inputs(m_input_procs, std::make_index_sequence<num_inputs>{})};
};

template <engine::graph_node Node, class... ChannelMappings>
auto
make_remap_input_channels(
        std::unique_ptr<Node> node,
        ChannelMappings... mappings) -> std::unique_ptr<engine::component>
{
    return std::make_unique<remap_input_channels<Node, ChannelMappings...>>(
            std::move(node),
            std::move(mappings)...);
}

} // namespace piejam::audio::components
