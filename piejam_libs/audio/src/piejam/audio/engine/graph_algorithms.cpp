// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/graph_algorithms.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_identity_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/functional/compare.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <ranges>

namespace piejam::audio::engine
{

namespace
{

auto
connected_source(graph::wires_t const& ws, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    auto it = std::ranges::find_if(
            ws,
            piejam::equal_to(dst),
            &graph::wires_t::value_type::second);
    if (it != ws.end())
        return it->first;
    else
        return {};
}

} // namespace

auto
connected_source(graph const& g, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    return connected_source(g.wires(), dst);
}

auto
connected_event_source(graph const& g, graph_endpoint const& dst)
        -> std::optional<graph_endpoint>
{
    return connected_source(g.event_wires(), dst);
}

bool
has_wire(graph const& g, graph_endpoint const& src, graph_endpoint const& dst)
{
    auto connected = g.wires().equal_range(src);
    return connected.first != connected.second &&
           std::ranges::find(
                   connected.first,
                   connected.second,
                   dst,
                   &graph::wires_t::value_type::second) != connected.second;
}

bool
has_event_wire(
        graph const& g,
        graph_endpoint const& src,
        graph_endpoint const& dst)
{
    auto connected = g.event_wires().equal_range(src);
    return connected.first != connected.second &&
           std::ranges::find(
                   connected.first,
                   connected.second,
                   dst,
                   &graph::wires_t::value_type::second) != connected.second;
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
                graph_endpoint prev_mixer_in{prev_mixer, in_index};
                auto const connected_in_src =
                        connected_source(g, prev_mixer_in);
                BOOST_ASSERT(connected_in_src);
                g.remove_wire(*connected_in_src, prev_mixer_in);
                g.add_wire(*connected_in_src, {*mixer, in_index});
            }

            g.add_wire(src, {*mixer, num_prev_inputs});

            g.remove_wire({prev_mixer, 0}, dst);
            g.add_wire({*mixer, 0}, dst);

            auto it_mixer = std::ranges::find_if(
                    mixers,
                    [pma = std::addressof(prev_mixer)](auto const& m) {
                        return m.get() == pma;
                    });
            BOOST_ASSERT(it_mixer != mixers.end());
            std::swap(*it_mixer, mixer);
        }
        else
        {
            g.remove_wire(*connected_src, dst);

            auto mixer = make_mix_processor(2);
            g.add_wire(*connected_src, {*mixer, 0});
            g.add_wire(src, {*mixer, 1});
            g.add_wire({*mixer, 0}, dst);
            mixers.emplace_back(std::move(mixer));
        }
    }
    else
    {
        g.add_wire(src, dst);
    }
}

void
connect_stereo_components(graph& g, component const& src, component const& dst)
{
    g.add_wire(src.outputs()[0], dst.inputs()[0]);
    g.add_wire(src.outputs()[1], dst.inputs()[1]);
}

void
connect_stereo_components(
        graph& g,
        component const& src,
        component const& dst,
        std::vector<std::unique_ptr<processor>>& mixers)
{
    connect(g, src.outputs()[0], dst.inputs()[0], mixers);
    connect(g, src.outputs()[1], dst.inputs()[1], mixers);
}

void
connect_stereo_components(graph& g, component const& src, processor& dst)
{
    g.add_wire(src.outputs()[0], graph_endpoint{.proc = dst, .port = 0});
    g.add_wire(src.outputs()[1], graph_endpoint{.proc = dst, .port = 1});
}

namespace
{

struct remove_event_identity_delegate
{
    static constexpr auto const is_identity = &is_event_identity_processor;
    static constexpr auto const wires = &graph::event_wires;
    static constexpr auto const add_wire = &graph::add_event_wire;
    static constexpr auto const remove_wire = &graph::remove_event_wire;

    template <std::predicate<graph_endpoint const&, graph_endpoint const&> P>
    static void remove_wires_if(graph& g, P&& p)
    {
        g.remove_event_wires_if(std::forward<P>(p));
    }
};

struct remove_identity_delegate
{
    static constexpr auto const is_identity = &is_identity_processor;
    static constexpr auto const wires = &graph::wires;
    static constexpr auto const add_wire = &graph::add_wire;
    static constexpr auto const remove_wire = &graph::remove_event_wire;

    template <std::predicate<graph_endpoint const&, graph_endpoint const&> P>
    static void remove_wires_if(graph& g, P&& p)
    {
        g.remove_wires_if(std::forward<P>(p));
    }
};

template <class Delegate>
void
remove_identities(graph& g)
{
    auto starts_in_identity = [](auto const& w) {
        return Delegate::is_identity(w.first.proc);
    };

    auto it = std::ranges::find_if(
            std::invoke(Delegate::wires, g),
            starts_in_identity);
    while (it != std::invoke(Delegate::wires, g).end())
    {
        auto it_ends = std::ranges::find_if(
                std::invoke(Delegate::wires, g),
                [p = &it->first.proc.get()](auto const& w) {
                    return &w.second.proc.get() == p;
                });

        auto out_wire = *it;
        std::invoke(Delegate::remove_wire, g, it);

        if (it_ends != std::invoke(Delegate::wires, g).end())
            std::invoke(Delegate::add_wire, g, it_ends->first, out_wire.second);

        it = std::ranges::find_if(
                std::invoke(Delegate::wires, g),
                starts_in_identity);
    }

    Delegate::remove_wires_if(g, [](auto const& /*src*/, auto const& dst) {
        return Delegate::is_identity(dst.proc);
    });
}

} // namespace

void
remove_event_identity_processors(graph& g)
{
    remove_identities<remove_event_identity_delegate>(g);
}

void
remove_identity_processors(graph& g)
{
    remove_identities<remove_identity_delegate>(g);
}

} // namespace piejam::audio::engine
