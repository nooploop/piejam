// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/graph_endpoint.h>

#include <boost/hof/unpack.hpp>

#include <concepts>
#include <iosfwd>
#include <map>

namespace piejam::audio::engine
{

class graph
{
public:
    class wires_map
    {
    public:
        using map_t = std::multimap<graph_endpoint, graph_endpoint>;
        using const_iterator = typename map_t::const_iterator;
        using value_type = typename map_t::value_type;

        [[nodiscard]]
        auto empty() const noexcept
        {
            return m_wires.empty();
        }

        [[nodiscard]]
        auto size() const noexcept
        {
            return m_wires.size();
        }

        [[nodiscard]]
        auto begin() const noexcept
        {
            return m_wires.begin();
        }

        [[nodiscard]]
        auto end() const noexcept
        {
            return m_wires.end();
        }

        [[nodiscard]]
        auto equal_range(graph_endpoint const& src) const noexcept
        {
            return m_wires.equal_range(src);
        }

        void erase(graph_endpoint const& src, graph_endpoint const& dst);
        auto erase(const_iterator const&) -> const_iterator;

        template <
                std::predicate<graph_endpoint const&, graph_endpoint const&> P>
        void erase_if(P&& p)
        {
            std::erase_if(m_wires, boost::hof::unpack(std::forward<P>(p)));
        }

    protected:
        map_t m_wires;
    };

    enum class wire_type
    {
        audio,
        event
    };

    template <wire_type W>
    class wires_access final : public wires_map
    {
    public:
        void insert(graph_endpoint const& src, graph_endpoint const& dst);
    };

    using audio_wires_access = wires_access<wire_type::audio>;
    using event_wires_access = wires_access<wire_type::event>;

    audio_wires_access audio;
    event_wires_access event;
};

using wire_t = graph::wires_map::value_type;

[[nodiscard]]
inline constexpr auto
src_processor(wire_t const& w) -> processor&
{
    return w.first.proc;
}

[[nodiscard]]
inline constexpr auto
src_port(wire_t const& w) -> std::size_t
{
    return w.first.port;
}

[[nodiscard]]
inline constexpr auto
dst_processor(wire_t const& w) -> processor&
{
    return w.second.proc;
}

[[nodiscard]]
inline constexpr auto
dst_port(wire_t const& w) -> std::size_t
{
    return w.second.port;
}

auto export_graph_as_dot(graph const&, std::ostream&) -> std::ostream&;

} // namespace piejam::audio::engine
