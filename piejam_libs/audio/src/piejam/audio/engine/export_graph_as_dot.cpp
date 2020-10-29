// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/audio/engine/graph.h>

#include <piejam/algorithm/unique_erase.h>
#include <piejam/functional/address_compare.h>

#include <fmt/format.h>

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
    procs.reserve((g.wires().size() + g.event_wires().size()) * 2);

    auto add_procs = [](auto& pc, auto&& w) {
        pc.push_back(w.first.proc);
        pc.push_back(w.second.proc);
    };

    for (auto const& w : g.wires())
        add_procs(procs, w);
    for (auto const& w : g.event_wires())
        add_procs(procs, w);

    std::ranges::sort(procs, address_less<processor>{});

    algorithm::unique_erase(procs, address_equal_to<processor>{});

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
        ss << p.type_name() << '_' << std::addressof(p) << " [label=<<table>"
           << std::endl;

        if (p.num_inputs() || p.num_event_inputs())
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
            for (std::size_t i = 0; i < p.num_event_inputs(); ++i)
            {
                ss << fmt::format(
                              "<td port=\"ei{}\" bgcolor=\"{}\">e{}</td>",
                              i,
                              event_color,
                              i)
                   << std::endl;
            }
            ss << "</tr>" << std::endl;
        }

        ss << "<tr><td colspan=\""
           << std::max(
                      p.num_inputs() + p.num_event_inputs(),
                      p.num_outputs() + p.num_event_outputs())
           << "\">" << p.type_name() << "</td></tr>" << std::endl;

        if (p.num_outputs() || p.num_event_outputs())
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
            for (std::size_t i = 0; i < p.num_event_outputs(); ++i)
            {
                ss << fmt::format(
                              "<td port=\"eo{}\" bgcolor=\"{}\">e{}</td>",
                              i,
                              event_color,
                              i)
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

    for (auto const& w : g.wires())
        print_wire(ss, w, 'a', audio_color);

    for (auto const& w : g.event_wires())
        print_wire(ss, w, 'e', event_color);

    ss << "}" << std::endl; // digraph
    return ss.str();
}

} // namespace piejam::audio::engine