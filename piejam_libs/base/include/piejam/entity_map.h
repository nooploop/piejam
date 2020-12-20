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

#pragma once

#include <piejam/entity_id.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

#include <concepts>

namespace piejam
{

template <class Entity, class EntityTag = Entity>
class entity_map
{
public:
    using id_t = entity_id<EntityTag>;
    using map_t = boost::container::flat_map<id_t, Entity>;

    auto empty() const noexcept { return m_map.empty(); }
    auto size() const noexcept { return m_map.size(); }

    auto begin() const noexcept { return m_map.begin(); }
    auto end() const noexcept { return m_map.end(); }

    auto begin() noexcept { return m_map.begin(); }
    auto end() noexcept { return m_map.end(); }

    auto contains(id_t id) const noexcept { return m_map.contains(id); }

    auto operator[](id_t id) const noexcept -> Entity const*
    {
        auto it = m_map.find(id);
        return it != m_map.end() ? &(it->second) : nullptr;
    }

    template <std::convertible_to<Entity> V>
    auto add(V&& v) -> id_t
    {
        auto id = id_t::generate();
        m_map.emplace_hint(m_map.end(), id, std::forward<V>(v));
        return id;
    }

    template <std::invocable<Entity&> U>
    auto update(id_t id, U&& u)
    {
        auto it = m_map.find(id);
        BOOST_ASSERT(it != m_map.end());
        return u(it->second);
    }

    auto remove(id_t id) { return m_map.erase(id); }

private:
    map_t m_map;
};

} // namespace piejam
