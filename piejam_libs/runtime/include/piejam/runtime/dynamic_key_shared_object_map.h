// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/functional/any_equal_to.h>
#include <piejam/functional/get.h>

#include <boost/assert.hpp>
#include <boost/hof/capture.hpp>

#include <algorithm>
#include <any>
#include <memory>
#include <vector>

namespace piejam::runtime
{

template <class T>
class dynamic_key_shared_object_map
{
    using mapping_pair = std::pair<std::any, std::shared_ptr<T>>;
    using map_t = std::vector<mapping_pair>;

public:
    template <class Id>
    auto find(Id const& id) const -> std::shared_ptr<T> const&
    {
        static std::shared_ptr<T> s_null_object;
        auto it = find_it(m_map, id);
        return it != m_map.end() ? it->second : s_null_object;
    }

    template <class Id>
    void insert(Id const& id, std::shared_ptr<T> obj)
    {
        BOOST_ASSERT(m_map.end() == find_it(m_map, id));
        m_map.emplace_back(id, std::move(obj));
    }

private:
    template <class Map, class Id>
    static auto find_it(Map&& m, Id const& id)
    {
        return std::ranges::find_if(
                std::forward<Map>(m),
                boost::hof::capture_forward(id)(any_equal_to),
                get_by_index<0>);
    }

    map_t m_map;
};

} // namespace piejam::runtime
