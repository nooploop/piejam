// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id_hash.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/dynamic_bitset.hpp>

namespace piejam::runtime
{

struct solo_group
{
    bool_parameter_id solo_param;
    boost::container::flat_set<mixer::channel_id> unmutes;
    boost::container::flat_set<mixer::channel_id> mutes;
};

using solo_groups_t = boost::container::flat_map<mixer::channel_id, solo_group>;

auto solo_groups(mixer::channels_t const&) -> solo_groups_t;

class solo_group_state
{
public:
    struct solo_group_bitsets
    {
        explicit solo_group_bitsets(std::size_t size)
            : mutes{size}
            , unmutes{size}
        {
        }

        boost::dynamic_bitset<> mutes;
        boost::dynamic_bitset<> unmutes;
    };

    explicit solo_group_state(solo_groups_t const& solo_groups);

    void set_solo(std::size_t channel_index, bool solo)
    {
        m_solo_state.set(channel_index, solo);
    }

    [[nodiscard]] auto any_solo() const noexcept -> bool
    {
        return m_solo_state.any();
    }

    void calculate_mutes() noexcept;

    [[nodiscard]] auto mutes() const noexcept -> boost::dynamic_bitset<> const&
    {
        return m_mutes;
    }

private:
    std::vector<solo_group_bitsets> m_solo_groups;

    boost::dynamic_bitset<> m_solo_state{m_solo_groups.size()};

    boost::dynamic_bitset<> m_mutes{m_solo_groups.size()};
    boost::dynamic_bitset<> m_unmutes{m_solo_groups.size()};
};

} // namespace piejam::runtime
