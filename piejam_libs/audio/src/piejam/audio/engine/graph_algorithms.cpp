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

void
connect(graph& g,
        graph_endpoint const& src,
        graph_endpoint const& dst,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    if (auto const connected_src = connected_source(g, dst))
    {
        if (is_mix_processor(connected_src->proc))
        {
            processor& prev_mixer = connected_src->proc;
            std::size_t const num_prev_inputs = prev_mixer.num_inputs();
            auto mixer = make_mix_processor(num_prev_inputs + 1);

            for (std::size_t in_index = 0; in_index < num_prev_inputs;
                 ++in_index)
            {
                graph_endpoint const prev_mixer_in{prev_mixer, in_index};
                auto const connected_in_src =
                        connected_source(g, prev_mixer_in);
                BOOST_ASSERT(connected_in_src);
                g.audio.erase(*connected_in_src, prev_mixer_in);
                g.audio.insert(*connected_in_src, {*mixer, in_index});
            }

            g.audio.insert(src, {*mixer, num_prev_inputs});

            g.audio.erase({prev_mixer, 0}, dst);
            g.audio.insert({*mixer, 0}, dst);

            auto it_mixer = std::ranges::find_if(
                    mixers,
                    address_equal_to<processor>(prev_mixer),
                    indirection_op);
            BOOST_ASSERT(it_mixer != mixers.end());
            std::swap(*it_mixer, mixer);
        }
        else
        {
            g.audio.erase(*connected_src, dst);

            auto mixer = make_mix_processor(2);
            g.audio.insert(*connected_src, {*mixer, 0});
            g.audio.insert(src, {*mixer, 1});
            g.audio.insert({*mixer, 0}, dst);
            mixers.emplace_back(std::move(mixer));
        }
    }
    else
    {
        g.audio.insert(src, dst);
    }
}

void
connect_stereo_components(graph& g, component const& src, component const& dst)
{
    using namespace endpoint_indices;
    connect(g, src, stereo, dst, stereo);
}

void
connect_stereo_components(
        graph& g,
        component const& src,
        component const& dst,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    using namespace endpoint_indices;
    connect(g, src, stereo, dst, stereo, mixers);
}

void
connect_stereo_components(graph& g, component const& src, processor& dst)
{
    using namespace endpoint_indices;
    connect(g, src, stereo, dst, stereo);
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
    auto starts_in_identity = [&](auto const& w) {
        return is_identity_processor<W>(w.first.proc);
    };

    auto it = std::ranges::find_if(g, starts_in_identity);
    while (it != g.end())
    {
        auto it_ends = std::ranges::find_if(
                g,
                address_equal_to<processor>(it->first.proc),
                &dst_processor);

        auto out_wire = *it;
        g.erase(it);

        if (it_ends != g.end())
        {
            g.insert(it_ends->first, out_wire.second);
        }

        it = std::ranges::find_if(g, starts_in_identity);
    }

    g.erase_if([&](auto const& /*src*/, auto const& dst) {
        return is_identity_processor<W>(dst.proc);
    });
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

} // namespace piejam::audio::engine
