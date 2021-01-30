// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/graph_endpoint.h>

#include <boost/hof/unpack.hpp>

#include <concepts>
#include <map>
#include <string>

namespace piejam::audio::engine
{

class graph
{
public:
    using wires_t = std::multimap<graph_endpoint, graph_endpoint>;

    auto wires() const noexcept -> wires_t const& { return m_wires; }
    void add_wire(graph_endpoint const& src, graph_endpoint const& dst);
    void remove_wire(graph_endpoint const& src, graph_endpoint const& dst);

    auto event_wires() const noexcept -> wires_t const&
    {
        return m_event_wires;
    }

    void add_event_wire(graph_endpoint const& src, graph_endpoint const& dst);

    void remove_event_wire(wires_t::const_iterator const&);

    template <std::predicate<graph_endpoint const&, graph_endpoint const&> P>
    void remove_event_wires_if(P&& p)
    {
        std::erase_if(m_event_wires, boost::hof::unpack(std::forward<P>(p)));
    }

private:
    wires_t m_wires;
    wires_t m_event_wires;
};

auto export_graph_as_dot(graph const&) -> std::string;

} // namespace piejam::audio::engine
