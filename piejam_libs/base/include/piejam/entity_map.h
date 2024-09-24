// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>
#include <piejam/entity_id.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

namespace piejam
{

template <class Entity>
class entity_map
{
public:
    using id_t = entity_id<Entity>;
    using map_t = boost::container::flat_map<id_t, Entity>;
    using value_type = typename map_t::value_type;

    [[nodiscard]]
    auto empty() const noexcept
    {
        return m_map->empty();
    }

    [[nodiscard]]
    auto size() const noexcept
    {
        return m_map->size();
    }

    [[nodiscard]]
    auto begin() const noexcept
    {
        return m_map->begin();
    }

    [[nodiscard]]
    auto end() const noexcept
    {
        return m_map->end();
    }

    [[nodiscard]]
    auto contains(id_t const id) const noexcept
    {
        return m_map->contains(id);
    }

    [[nodiscard]]
    auto find(id_t const id) const noexcept -> Entity const*
    {
        auto it = m_map->find(id);
        return it != m_map->end() ? std::addressof(it->second) : nullptr;
    }

    [[nodiscard]]
    auto operator[](id_t const id) const noexcept -> Entity const&
    {
        auto it = m_map->find(id);
        BOOST_ASSERT(it != m_map->end());
        return it->second;
    }

    class locked
    {
    public:
        explicit locked(entity_map& m)
            : m_{m.m_map.lock()}
        {
        }

        [[nodiscard]]
        auto begin() noexcept
        {
            return m_->begin();
        }

        [[nodiscard]]
        auto end() noexcept
        {
            return m_->end();
        }

        [[nodiscard]]
        auto operator[](id_t const id) -> Entity&
        {
            auto it = m_->find(id);
            BOOST_ASSERT(it != m_->end());
            return it->second;
        }

        [[nodiscard]]
        auto insert(Entity value) -> id_t
        {
            auto id = id_t::generate();
            m_->emplace_hint(m_->end(), id, std::move(value));
            return id;
        }

        template <class... Args>
        [[nodiscard]]
        auto emplace(Args&&... args) -> id_t
        {
            auto id = id_t::generate();
            m_->emplace_hint(
                    m_->end(),
                    std::piecewise_construct,
                    std::forward_as_tuple(id),
                    std::forward_as_tuple(std::forward<Args>(args)...));
            return id;
        }

        auto erase(id_t const id) -> typename map_t::size_type
        {
            return m_->erase(id);
        }

        template <std::ranges::range RangeOfIds>
        void erase(RangeOfIds&& ids)
        {
            for (auto id : ids)
            {
                m_->erase(id);
            }
        }

    private:
        box<map_t>::write_lock m_;
    };

    auto lock() -> locked
    {
        return locked{*this};
    }

    [[nodiscard]]
    auto insert(Entity value) -> id_t
    {
        return lock().insert(std::move(value));
    }

    template <class... Args>
    [[nodiscard]]
    auto emplace(Args&&... args) -> id_t
    {
        return lock().emplace(std::forward<Args>(args)...);
    }

    auto erase(id_t const id) -> typename map_t::size_type
    {
        return lock().erase(id);
    }

    template <std::ranges::range RangeOfIds>
    void erase(RangeOfIds&& ids)
    {
        lock().erase(std::forward<RangeOfIds>(ids));
    }

    auto operator==(entity_map const&) const noexcept -> bool = default;

private:
    box<map_t> m_map;
};

} // namespace piejam
