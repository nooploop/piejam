// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/solo_group.h>

#include <piejam/runtime/mixer.h>

#include <piejam/range/indices.h>

namespace piejam::runtime
{

namespace
{

struct channel_info
{
    bool_parameter_id solo_param;
    boost::container::flat_set<mixer::channel_id> children;
    boost::container::flat_set<mixer::channel_id> mixins;
};

using channel_infos =
        boost::container::flat_map<mixer::channel_id, channel_info>;

auto
gather_channel_infos(mixer::channels_t const& channels)
{
    std::size_t const num_channels = channels.size();

    channel_infos result;
    result.reserve(num_channels);

    auto add = [&](mixer::channel_id current, mixer::io_address_t const& out) {
        if (auto const* const target_id = std::get_if<mixer::channel_id>(&out);
            target_id)
        {
            auto const* const target_channel = channels.find(*target_id);
            BOOST_ASSERT(target_channel);

            if (std::holds_alternative<default_t>(target_channel->in))
            {
                result[current].children.insert(*target_id);
                result[*target_id].mixins.insert(current);
            }
        }
    };

    for (auto const& [id, channel] : channels)
    {
        auto& info = result[id];

        info.children.reserve(num_channels);
        info.mixins.reserve(num_channels);
        info.solo_param = channel.solo;

        if (auto const* const in_id =
                    std::get_if<mixer::channel_id>(&channel.in);
            in_id)
        {
            result[*in_id].children.insert(id);
        }

        add(id, channel.out);

        for (auto const& [aux, aux_send] : *channel.aux_sends)
        {
            if (aux_send.enabled)
            {
                add(id, aux);
            }
        }
    }

    return result;
}

void
gather_solo_group(
        mixer::channel_id id,
        channel_infos const& infos,
        solo_group& g)
{
    auto info = infos.find(id);
    BOOST_ASSERT(info != infos.end());
    g.mutes.insert(
            boost::container::ordered_unique_range,
            info->second.mixins.begin(),
            info->second.mixins.end());
    g.unmutes.insert(id);

    for (auto child : info->second.children)
    {
        gather_solo_group(child, infos, g);
    }
}

} // namespace

auto
solo_groups(mixer::channels_t const& channels) -> solo_groups_t
{
    auto infos = gather_channel_infos(channels);

    std::size_t const num_channels = channels.size();

    solo_groups_t result;
    result.reserve(num_channels);

    for (auto const& [id, info] : infos)
    {
        auto& g = result[id];
        g.solo_param = info.solo_param;
        g.mutes.reserve(num_channels);
        g.unmutes.reserve(num_channels);

        gather_solo_group(id, infos, g);
    }

    return result;
}

static auto
to_solo_group_bitsets(solo_groups_t const& solo_groups)
{
    std::vector<solo_group_state::solo_group_bitsets> result;
    result.reserve(solo_groups.size());

    for (auto const& [channel_id, group] : solo_groups)
    {
        auto& bitsets = result.emplace_back(solo_groups.size());

        for (auto id : group.mutes)
        {
            bitsets.mutes.set(solo_groups.index_of(solo_groups.find(id)));
        }

        for (auto id : group.unmutes)
        {
            bitsets.unmutes.set(solo_groups.index_of(solo_groups.find(id)));
        }
    }

    return result;
}

solo_group_state::solo_group_state(solo_groups_t const& solo_groups)
    : m_solo_groups{to_solo_group_bitsets(solo_groups)}
{
}

void
solo_group_state::calculate_mutes() noexcept
{
    m_mutes.reset();
    m_unmutes.reset();

    for (std::size_t const index : range::indices(m_solo_groups))
    {
        if (m_solo_state.test(index))
        {
            auto const& group = m_solo_groups[index];
            m_mutes |= group.mutes;
            m_unmutes |= group.unmutes;
        }
    }

    m_mutes -= m_unmutes;
}

} // namespace piejam::runtime
