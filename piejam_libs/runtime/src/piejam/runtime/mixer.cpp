// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/mixer.h>

#include <piejam/io_pair.h>

#include <boost/hof/unpack.hpp>

#include <algorithm>
#include <ranges>

namespace piejam::runtime::mixer
{

namespace
{

using channel_io_t = io_pair<io_address_t>;
using channels_io_t = boost::container::flat_map<channel_id, channel_io_t>;

auto
extract_channels_io(channels_t const& channels) -> channels_io_t
{
    auto transformed = std::views::transform(
            channels,
            boost::hof::unpack([](auto const id, auto const& channel) {
                return std::pair(id, channel_io_t(channel.in, channel.out));
            }));

    return channels_io_t(
            boost::container::ordered_unique_range,
            transformed.begin(),
            transformed.end());
}

struct io_graph_node
{
    std::vector<channel_id> children;
    bool finished{};
    bool visited{};
};

using io_graph = boost::container::flat_map<channel_id, io_graph_node>;

auto
make_channels_io_graph(channels_io_t const& channels_io) -> io_graph
{
    io_graph result;
    result.reserve(channels_io.size());

    for (auto const& [id, ch_io] : channels_io)
    {
        result[id];

        if (channel_id const* const in_channel_id =
                    std::get_if<channel_id>(&ch_io.in))
        {
            result[*in_channel_id].children.push_back(id);
        }

        if (channel_id const* const out_channel_id =
                    std::get_if<channel_id>(&ch_io.out))
        {
            if (std::holds_alternative<default_t>(
                        channels_io.at(*out_channel_id).in))
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
    {
        return false;
    }

    if (node.visited)
    {
        return true;
    }

    node.visited = true;

    for (auto child : node.children)
    {
        if (has_cycle(g, child))
        {
            return true;
        }
    }

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
    auto channels_io = extract_channels_io(channels);

    std::vector<mixer::channel_id> valid_ids;
    for (auto const& [mixer_channel_id, mixer_channel] : channels)
    {
        if (mixer_channel_id == ch_id)
        {
            // mono mixer channels can't have input channels
            if (D == io_direction::input &&
                mixer_channel.bus_type == audio::bus_type::mono)
            {
                return {};
            }

            // otherwise, we can't be our own input
            continue;
        }

        auto prev_id = channels_io[ch_id].get(D);
        channels_io[ch_id].get(D) = mixer_channel_id;

        if (!has_cycle(make_channels_io_graph(channels_io)))
        {
            valid_ids.push_back(mixer_channel_id);
        }

        channels_io[ch_id].get(D) = prev_id;
    }

    return valid_ids;
}

} // namespace

bool
is_default_source_valid(channels_t const& channels, channel_id const ch_id)
{
    auto channels_io = extract_channels_io(channels);
    channels_io[ch_id].in = default_t{};
    return !has_cycle(make_channels_io_graph(channels_io));
}

auto
valid_source_channels(channels_t const& channels, channel_id const ch_id)
        -> std::vector<channel_id>
{
    return valid_io_channels<io_direction::input>(channels, ch_id);
}

auto
valid_target_channels(channels_t const& channels, channel_id const ch_id)
        -> std::vector<channel_id>
{
    return valid_io_channels<io_direction::output>(channels, ch_id);
}

} // namespace piejam::runtime::mixer
