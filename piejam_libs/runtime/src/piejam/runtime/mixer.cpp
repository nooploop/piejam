// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/mixer.h>

#include <piejam/io_pair.h>

#include <boost/hof/unpack.hpp>

#include <algorithm>

namespace piejam::runtime::mixer
{

namespace
{

using bus_ios_t = boost::container::flat_map<bus_id, bus_io_t>;

auto
extract_bus_ios(buses_t const& buses) -> bus_ios_t
{
    bus_ios_t result;
    result.reserve(buses.size());

    std::ranges::transform(
            buses,
            std::inserter(result, result.end()),
            boost::hof::unpack([](auto const id, auto const& bus) {
                return std::pair(id, bus_io_t(bus.in, bus.out));
            }));

    return result;
}

struct io_graph_node
{
    std::vector<bus_id> children;
    bool finished{};
    bool visited{};
};

using io_graph = boost::container::flat_map<bus_id, io_graph_node>;

auto
make_bus_io_graph(bus_ios_t const& bus_ios) -> io_graph
{
    io_graph result;
    result.reserve(bus_ios.size());

    for (auto const& [id, bus_io] : bus_ios)
    {
        result[id];

        if (bus_id const* const in_bus_id = std::get_if<bus_id>(&bus_io.in))
        {
            result[*in_bus_id].children.push_back(id);
        }

        if (bus_id const* const out_bus_id = std::get_if<bus_id>(&bus_io.out))
        {
            if (std::holds_alternative<std::nullptr_t>(
                        bus_ios.at(*out_bus_id).in))
            {
                result[id].children.push_back(*out_bus_id);
            }
        }
    }

    return result;
}

bool
has_cycle(io_graph& g, bus_id const id)
{
    BOOST_ASSERT(g.contains(id));

    auto& node = g[id];

    if (node.finished)
        return false;

    if (node.visited)
        return true;

    node.visited = true;

    // für jeden Nachfolger w DFS(w)
    for (auto child : node.children)
        if (has_cycle(g, child))
            return true;

    node.finished = true;

    return false;
}

bool
has_cycle(io_graph g)
{
    return std::ranges::any_of(g, [&g](auto const& id_node) {
        return has_cycle(g, id_node.first);
    });
}

template <io_direction D>
auto
valid_io_channels(buses_t const& mixer_buses, bus_id const mixer_bus_id)
        -> std::vector<mixer::bus_id>
{
    auto const current = extract_bus_ios(mixer_buses);

    std::vector<mixer::bus_id> valid_ids;
    for (auto const& [id, bus] : mixer_buses)
    {
        if (id == mixer_bus_id)
            continue;

        auto test = current;
        test[mixer_bus_id].get(D) = id;

        if (!has_cycle(make_bus_io_graph(test)))
            valid_ids.push_back(id);
    }

    return valid_ids;
}

} // namespace

bool
is_default_source_valid(buses_t const& mixer_buses, bus_id const mixer_bus_id)
{
    auto test = extract_bus_ios(mixer_buses);
    test[mixer_bus_id].in = nullptr; // default case
    return !has_cycle(make_bus_io_graph(test));
}

auto
valid_source_channels(buses_t const& mixer_buses, bus_id const mixer_bus_id)
        -> std::vector<mixer::bus_id>
{
    return valid_io_channels<io_direction::input>(mixer_buses, mixer_bus_id);
}

auto
valid_target_channels(buses_t const& mixer_buses, bus_id const mixer_bus_id)
        -> std::vector<mixer::bus_id>
{
    return valid_io_channels<io_direction::output>(mixer_buses, mixer_bus_id);
}

} // namespace piejam::runtime::mixer
