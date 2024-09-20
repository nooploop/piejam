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
    using value_type = typename map_t::value_type;

    [[nodiscard]] auto empty() const noexcept
    {
        return m_map->empty();
    }

    [[nodiscard]] auto size() const noexcept
    {
        return m_map->size();
    }

    [[nodiscard]] auto begin() const noexcept
    {
        return m_map->begin();
    }

    [[nodiscard]] auto end() const noexcept
    {
        return m_map->end();
    }

    [[nodiscard]] auto contains(id_t const id) const noexcept
    {
        return m_map->contains(id);
    }

    [[nodiscard]] auto find(id_t const id) const noexcept -> Entity const*
    {
        auto it = m_map->find(id);
        return it != m_map->end() ? std::addressof(it->second) : nullptr;
    }

    [[nodiscard]] auto operator[](id_t const id) const noexcept -> Entity const&
    {
        auto it = m_map->find(id);
        BOOST_ASSERT(it != m_map->end());
        return it->second;
    }

    template <class... Args>
    [[nodiscard]] auto add(Args&&... args) -> id_t
    {
        auto id = id_t::generate();

        auto m = m_map.lock();
        m->emplace_hint(
                m->end(),
                std::piecewise_construct,
                std::forward_as_tuple(id),
                std::forward_as_tuple(std::forward<Args>(args)...));
        return id;
    }

    template <std::invocable<Entity&> U>
    auto update(id_t const id, U&& u)
    {
        auto m = m_map.lock();

        auto it = m->find(id);
        BOOST_ASSERT(it != m->end());
        return u(it->second);
    }

    template <std::invocable<id_t, Entity&> U>
    auto update(std::span<id_t const> const ids, U&& u)
    {
        auto m = m_map.lock();

        for (auto const id : ids)
        {
            auto it = m->find(id);
            BOOST_ASSERT(it != m->end());
            u(id, it->second);
        }
    }

    template <std::invocable<id_t, Entity&> U>
    auto update(U&& u)
    {
        auto m = m_map.lock();

        for (auto&& [id, value] : *m)
        {
            u(id, value);
        }
    }

    auto remove(id_t const id) -> typename map_t::size_type
    {
        return m_map.lock()->erase(id);
    }

    template <std::ranges::range RangeOfIds>
    void remove(RangeOfIds const& ids)
    {
        auto m = m_map.lock();

        for (id_t const id : ids)
        {
            m->erase(id);
        }
    }

    auto operator==(entity_map const&) const noexcept -> bool = default;

private:
    box<map_t> m_map;
};

} // namespace piejam
