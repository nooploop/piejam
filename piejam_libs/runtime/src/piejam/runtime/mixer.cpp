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

using channel_ios_t = boost::container::flat_map<channel_id, channel_io_t>;

auto
extract_bus_ios(channels_t const& channels) -> channel_ios_t
{
    channel_ios_t result;
    result.reserve(channels.size());

    std::ranges::transform(
            channels,
            std::inserter(result, result.end()),
            boost::hof::unpack([](auto const id, auto const& channel) {
                return std::pair(id, channel_io_t(channel.in, channel.out));
            }));

    return result;
}

struct io_graph_node
{
    std::vector<channel_id> children;
    bool finished{};
    bool visited{};
};

using io_graph = boost::container::flat_map<channel_id, io_graph_node>;

auto
make_bus_io_graph(channel_ios_t const& bus_ios) -> io_graph
{
    io_graph result;
    result.reserve(bus_ios.size());

    for (auto const& [id, bus_io] : bus_ios)
    {
        result[id];

        if (channel_id const* const in_channel_id =
                    std::get_if<channel_id>(&bus_io.in))
        {
            result[*in_channel_id].children.push_back(id);
        }

        if (channel_id const* const out_channel_id =
                    std::get_if<channel_id>(&bus_io.out))
        {
            if (std::holds_alternative<std::nullptr_t>(
                        bus_ios.at(*out_channel_id).in))
            {
                result[id].children.push_back(*out_channel_id);
            }
        }
    }

    return result;
}

bool
has_cycle(io_graph& g, channel_id const id)
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
valid_io_channels(channels_t const& channels, channel_id const ch_id)
        -> std::vector<mixer::channel_id>
{
    auto const current = extract_bus_ios(channels);

    std::vector<mixer::channel_id> valid_ids;
    for (auto const& [id, bus] : channels)
    {
        if (id == ch_id)
            continue;

        auto test = current;
        test[ch_id].get(D) = id;

        if (!has_cycle(make_bus_io_graph(test)))
            valid_ids.push_back(id);
    }

    return valid_ids;
}

} // namespace

bool
is_default_source_valid(channels_t const& channels, channel_id const ch_id)
{
    auto test = extract_bus_ios(channels);
    test[ch_id].in = nullptr; // default case
    return !has_cycle(make_bus_io_graph(test));
}

auto
valid_source_channels(channels_t const& channels, channel_id const ch_id)
        -> std::vector<mixer::channel_id>
{
    return valid_io_channels<io_direction::input>(channels, ch_id);
}

auto
valid_target_channels(channels_t const& channels, channel_id const ch_id)
        -> std::vector<mixer::channel_id>
{
    return valid_io_channels<io_direction::output>(channels, ch_id);
}

} // namespace piejam::runtime::mixer
