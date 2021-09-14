// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/graph.h>

#include <piejam/algorithm/escape_html.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/functional/address_compare.h>

#include <fmt/format.h>

#include <boost/range/algorithm_ext/erase.hpp>

#include <algorithm>
#include <ranges>
#include <sstream>
#include <unordered_set>
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
        add_procs(procs, w);
    for (auto const& w : g.event)
        add_procs(procs, w);

    std::ranges::sort(procs, address_less<processor>{});

    boost::unique_erase(procs, address_equal_to<processor>{});

    return procs;
}

auto
export_graph_as_dot(graph const& g) -> std::string
{
    std::stringstream ss;
    ss << "digraph {" << std::endl;

    auto all_procs = graph_processors(g);

    ss << "node [shape=plaintext]" << std::endl;

    constexpr auto const audio_color = "#00ff00";
    constexpr auto const event_color = "#ff00ff";

    for (processor const& p : all_procs)
    {
        auto const num_event_inputs = p.event_inputs().size();
        auto const num_event_outputs = p.event_outputs().size();

        ss << p.type_name() << '_' << std::addressof(p) << " [label=<<table>"
           << std::endl;

        if (p.num_inputs() || num_event_inputs)
        {
            ss << "<tr>" << std::endl;
            for (std::size_t i = 0; i < p.num_inputs(); ++i)
            {
                ss << fmt::format(
                              "<td port=\"ai{}\" bgcolor=\"{}\">a{}</td>",
                              i,
                              audio_color,
                              i)
                   << std::endl;
            }
            for (std::size_t i = 0; i < num_event_inputs; ++i)
            {
                ss << fmt::format(
                              "<td port=\"ei{}\" bgcolor=\"{}\">{}</td>",
                              i,
                              event_color,
                              algorithm::escape_html(
                                      p.event_inputs()[i].name()))
                   << std::endl;
            }
            ss << "</tr>" << std::endl;
        }

        ss << "<tr>" << std::endl;
        ss << fmt::format(
                "<td colspan=\"{}\">{}:{}</td>",
                std::max(
                        p.num_inputs() + num_event_inputs,
                        p.num_outputs() + p.event_outputs().size()),
                p.type_name(),
                algorithm::escape_html(p.name()));
        ss << "</tr>" << std::endl;

        if (p.num_outputs() || num_event_outputs)
        {
            ss << "<tr>" << std::endl;
            for (std::size_t i = 0; i < p.num_outputs(); ++i)
            {
                ss << fmt::format(
                              "<td port=\"ao{}\" bgcolor=\"{}\">a{}</td>",
                              i,
                              audio_color,
                              i)
                   << std::endl;
            }
            for (std::size_t i = 0; i < num_event_outputs; ++i)
            {
                ss << fmt::format(
                              "<td port=\"eo{}\" bgcolor=\"{}\">{}</td>",
                              i,
                              event_color,
                              algorithm::escape_html(
                                      p.event_outputs()[i].name()))
                   << std::endl;
            }
            ss << "</tr>" << std::endl;
        }

        ss << "</table>>]" << std::endl;
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
           << std::endl;
    };

    for (auto const& w : g.audio)
        print_wire(ss, w, 'a', audio_color);

    for (auto const& w : g.event)
        print_wire(ss, w, 'e', event_color);

    ss << "}" << std::endl; // digraph
    return ss.str();
}

} // namespace piejam::audio::engine
