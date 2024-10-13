// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/graph.h>

#include <piejam/algorithm/escape_html.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/functional/address_compare.h>

#include <fmt/format.h>

#include <boost/range/algorithm_ext/erase.hpp>

#include <algorithm>
#include <ostream>
#include <vector>

namespace piejam::audio::engine
{

static auto
graph_processors(graph const& g)
{
    std::vector<std::reference_wrapper<processor>> procs;
    procs.reserve((g.audio.size() + g.event.size()) * 2);

    auto add_procs = [](auto& vec, auto&& w) {
        vec.push_back(w.first.proc);
        vec.push_back(w.second.proc);
    };

    for (auto const& w : g.audio)
    {
        add_procs(procs, w);
    }
    for (auto const& w : g.event)
    {
        add_procs(procs, w);
    }

    std::ranges::sort(procs, address_less<processor>);

    boost::unique_erase(procs, address_equal_to<processor>);

    return procs;
}

auto
export_graph_as_dot(graph const& g, std::ostream& os) -> std::ostream&
{
    os << "digraph {" << '\n';

    auto all_procs = graph_processors(g);

    os << "node [shape=plaintext]" << '\n';

    constexpr auto audio_color = "#00ff00";
    constexpr auto event_color = "#ff00ff";

    for (processor const& p : all_procs)
    {
        auto const num_event_inputs = p.event_inputs().size();
        auto const num_event_outputs = p.event_outputs().size();

        os << p.type_name() << '_' << std::addressof(p) << " [label=<<table>"
           << '\n';

        if (p.num_inputs() || num_event_inputs)
        {
            os << "<tr>" << '\n';
            for (std::size_t i = 0; i < p.num_inputs(); ++i)
            {
                os << fmt::format(
                              R"(<td port="ai{}" bgcolor="{}">a{}</td>)",
                              i,
                              audio_color,
                              i)
                   << '\n';
            }
            for (std::size_t i = 0; i < num_event_inputs; ++i)
            {
                os << fmt::format(
                              R"(<td port="ei{}" bgcolor="{}">{}</td>)",
                              i,
                              event_color,
                              algorithm::escape_html(
                                      p.event_inputs()[i].name()))
                   << '\n';
            }
            os << "</tr>" << '\n';
        }

        os << "<tr>" << '\n';
        os << fmt::format(
                "<td colspan=\"{}\">{}:{}</td>",
                std::max(
                        p.num_inputs() + num_event_inputs,
                        p.num_outputs() + p.event_outputs().size()),
                p.type_name(),
                algorithm::escape_html(p.name()));
        os << "</tr>" << '\n';

        if (p.num_outputs() || num_event_outputs)
        {
            os << "<tr>" << '\n';
            for (std::size_t i = 0; i < p.num_outputs(); ++i)
            {
                os << fmt::format(
                              R"(<td port="ao{}" bgcolor="{}">a{}</td>)",
                              i,
                              audio_color,
                              i)
                   << '\n';
            }
            for (std::size_t i = 0; i < num_event_outputs; ++i)
            {
                os << fmt::format(
                              R"(<td port="eo{}" bgcolor="{}">{}</td>)",
                              i,
                              event_color,
                              algorithm::escape_html(
                                      p.event_outputs()[i].name()))
                   << '\n';
            }
            os << "</tr>" << '\n';
        }

        os << "</table>>]" << '\n';
    }

    constexpr auto print_wire = [](auto& ss,
                                   auto const& w,
                                   auto&& wire_type,
                                   auto&& color) {
        ss << fmt::format(
                      "{}_{}:{}o{} -> {}_{}:{}i{} [color=\"{}\"]",
                      w.first.proc.get().type_name(),
                      static_cast<void*>(std::addressof(w.first.proc.get())),
                      wire_type,
                      w.first.port,
                      w.second.proc.get().type_name(),
                      static_cast<void*>(std::addressof(w.second.proc.get())),
                      wire_type,
                      w.second.port,
                      color)
           << '\n';
    };

    for (auto const& w : g.audio)
    {
        print_wire(os, w, 'a', audio_color);
    }

    for (auto const& w : g.event)
    {
        print_wire(os, w, 'e', event_color);
    }

    os << "}" << '\n'; // digraph

    return os;
}

} // namespace piejam::audio::engine
