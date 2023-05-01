// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/graph_algorithms.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_identity_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/processor.h>

#include <piejam/algorithm/contains.h>
#include <piejam/functional/address_compare.h>
#include <piejam/functional/operators.h>

#include <boost/assert.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <algorithm>
#include <ranges>
#include <set>

namespace piejam::audio::engine
{

namespace
{

auto
connected_source(graph::wires_map const& ws, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    auto it = std::ranges::find(ws, dst, &graph::wires_map::value_type::second);
    if (it != ws.end())
    {
        return it->first;
    }

    return std::nullopt;
}

auto
has_wire(
        graph::wires_map const& ws,
        graph_endpoint const& src,
        graph_endpoint const& dst) -> bool
{
    return algorithm::contains(
            boost::make_iterator_range(ws.equal_range(src)),
            dst,
            &graph::wires_map::value_type::second);
}

} // namespace

auto
connected_source(graph const& g, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    return connected_source(g.audio, dst);
}

auto
connected_event_source(graph const& g, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    return connected_source(g.event, dst);
}

auto
has_audio_wire(
        graph const& g,
        graph_endpoint const& src,
        graph_endpoint const& dst) -> bool
{
    return has_wire(g.audio, src, dst);
}

auto
has_event_wire(
        graph const& g,
        graph_endpoint const& src,
        graph_endpoint const& dst) -> bool
{
    return has_wire(g.event, src, dst);
}

namespace
{

template <graph::wire_type W>
auto
is_identity_processor(processor const& p) noexcept -> bool
{
    if constexpr (W == graph::wire_type::audio)
    {
        return is_identity_processor(p);
    }
    else
    {
        static_assert(W == graph::wire_type::event, "Unknown wire type.");
        return is_event_identity_processor(p);
    }
}

template <graph::wire_type W>
void
remove_identities(graph::wires_access<W>& g)
{
    typename graph::wires_access<W>::wires_map::map_t id_wires;

    for (auto it = g.begin(); it != g.end();)
    {
        if (is_identity_processor<W>(it->first.proc) ||
            is_identity_processor<W>(it->second.proc))
        {
            id_wires.emplace(*it);
            it = g.erase(it);
        }
        else
        {
            ++it;
        }
    }

    std::set<typename graph::wires_access<W>::wires_map::value_type> new_wires;

    for (auto it = id_wires.begin(); it != id_wires.end();)
    {
        if (is_identity_processor<W>(it->second.proc))
        {
            auto following = id_wires.equal_range(it->second);

            for (auto const& [src, dst] : boost::make_iterator_range(following))
            {
                if (is_identity_processor<W>(it->first.proc) ||
                    is_identity_processor<W>(dst.proc))
                {
                    id_wires.emplace(it->first, dst);
                }
                else
                {
                    new_wires.emplace(it->first, dst);
                }
            }

            it = id_wires.erase(it);

            if (following.first != following.second)
            {
                it = id_wires.begin();
            }
        }
        else
        {
            ++it;
        }
    }

    for (auto const& [src, dst] : new_wires)
    {
        g.insert(src, dst);
    }
}

auto
insert_mixer(graph& g) -> mix_processors
{
    mix_processors result;

    std::multimap<graph_endpoint, graph::wires_map::const_iterator> rev_g;

    // build the reverse graph
    for (auto it = g.audio.begin(), it_end = g.audio.end(); it != it_end; ++it)
    {
        rev_g.emplace(it->second, it);
    }

    for (auto it = rev_g.begin(), it_end = rev_g.end(); it != it_end;)
    {
        auto it_up = it;
        while (it->first == it_up->first)
        {
            ++it_up;
        }

        auto num_ins = static_cast<std::size_t>(std::distance(it, it_up));
        if (num_ins > 1)
        {
            auto dst = it->first;
            auto mixer = make_mix_processor(num_ins);
            std::size_t port{};
            while (it != it_up)
            {
                g.audio.insert(
                        it->second->first,
                        graph_endpoint{.proc = *mixer, .port = port++});
                g.audio.erase(it->second);
                ++it;
            }

            g.audio.insert(graph_endpoint{.proc = *mixer, .port = 0}, dst);

            result.emplace_back(std::move(mixer));
        }
        else
        {
            ++it;
        }
    }

    return result;
}

} // namespace

void
remove_event_identity_processors(graph& g)
{
    remove_identities(g.event);
}

void
remove_identity_processors(graph& g)
{
    remove_identities(g.audio);
}

auto
finalize_graph(graph const& g) -> std::tuple<graph, mix_processors>
{
    graph result{g};

    remove_event_identity_processors(result);
    remove_identity_processors(result);

    mix_processors mixers = insert_mixer(result);

    return std::tuple{std::move(result), std::move(mixers)};
}

} // namespace piejam::audio::engine
