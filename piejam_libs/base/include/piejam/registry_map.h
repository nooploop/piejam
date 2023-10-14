// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

namespace piejam
{

template <class Tag, class Key, class Value>
class registry_map
{
public:
    using map_t = boost::container::flat_map<Key, Value>;

    static void add_entry(Key key, Value value)
    {
        BOOST_VERIFY(
                mapping().emplace(std::move(key), std::move(value)).second);
    }

    static auto lookup(Key const& key) -> Value const&
    {
        auto it = mapping().find(key);
        BOOST_ASSERT(it != mapping().end());

        return it->second;
    }

private:
    static auto mapping() -> map_t&
    {
        static map_t s_map;
        return s_map;
    }
};

} // namespace piejam
