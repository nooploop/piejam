// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>
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

    auto empty() const noexcept { return m_map->empty(); }
    auto size() const noexcept { return m_map->size(); }

    auto begin() const noexcept { return m_map->begin(); }
    auto end() const noexcept { return m_map->end(); }

    auto contains(id_t id) const noexcept { return m_map->contains(id); }

    auto operator[](id_t id) const noexcept -> Entity const*
    {
        auto it = m_map->find(id);
        return it != m_map->end() ? &(it->second) : nullptr;
    }

    template <std::convertible_to<Entity> V>
    auto add(V&& v) -> id_t
    {
        auto id = id_t::generate();
        m_map.update([id, &v](map_t& m) {
            m.emplace_hint(m.end(), id, std::forward<V>(v));
        });
        return id;
    }

    template <std::invocable<Entity&> U>
    auto update(id_t id, U&& u)
    {
        return m_map.update([id, &u](map_t& m) {
            auto it = m.find(id);
            BOOST_ASSERT(it != m.end());
            return u(it->second);
        });
    }

    auto remove(id_t id) -> typename map_t::size_type
    {
        return m_map.update([id](map_t& m) { return m.erase(id); });
    }

    bool operator==(entity_map const& other) const noexcept
    {
        return m_map.eq(other.m_map);
    }

    bool operator!=(entity_map const& other) const noexcept
    {
        return m_map.neq(other.m_map);
    }

private:
    box<map_t> m_map;
};

} // namespace piejam
