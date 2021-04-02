// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>
#include <piejam/entity_id.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

#include <concepts>
#include <span>

namespace piejam
{

template <class Entity>
class entity_map
{
public:
    using id_t = entity_id<Entity>;
    using map_t = boost::container::flat_map<id_t, Entity>;

    auto empty() const noexcept { return m_map->empty(); }
    auto size() const noexcept { return m_map->size(); }

    auto begin() const noexcept { return m_map->begin(); }
    auto end() const noexcept { return m_map->end(); }

    auto contains(id_t const id) const noexcept { return m_map->contains(id); }

    auto find(id_t const id) const noexcept -> Entity const*
    {
        auto it = m_map->find(id);
        return it != m_map->end() ? std::addressof(it->second) : nullptr;
    }

    auto operator[](id_t const id) const noexcept -> Entity const&
    {
        auto it = m_map->find(id);
        BOOST_ASSERT(it != m_map->end());
        return it->second;
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

    auto add() -> id_t { return add(Entity{}); }

    template <std::invocable<Entity&> U>
    auto update(id_t const id, U&& u)
    {
        return m_map.update([id, &u](map_t& m) {
            auto it = m.find(id);
            BOOST_ASSERT(it != m.end());
            return u(it->second);
        });
    }

    template <std::invocable<id_t, Entity&> U>
    auto update(std::span<id_t const> const& ids, U&& u)
    {
        m_map.update([ids, &u](map_t& m) {
            for (auto const id : ids)
            {
                if (auto it = m.find(id); it != m.end())
                    u(id, it->second);
            }
        });
    }

    template <std::invocable<id_t, Entity&> U>
    auto update(U&& u)
    {
        m_map.update([&u](map_t& m) {
            for (auto&& [id, value] : m)
                u(id, value);
        });
    }

    auto remove(id_t const id) -> typename map_t::size_type
    {
        return m_map.update([id](map_t& m) { return m.erase(id); });
    }

    bool operator==(entity_map const&) const noexcept = default;

private:
    box<map_t> m_map;
};

} // namespace piejam
