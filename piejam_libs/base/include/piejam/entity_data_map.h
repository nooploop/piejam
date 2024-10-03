// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

#include <memory>

namespace piejam
{

template <class Id, class Data>
class entity_data_map
{
public:
    using id_t = Id;
    using value_type = Data;

    [[nodiscard]]
    auto empty() const noexcept
    {
        return m_map.empty();
    }

    [[nodiscard]]
    auto size() const noexcept
    {
        return m_map.size();
    }

    [[nodiscard]]
    auto contains(id_t id) const noexcept
    {
        return m_map.contains(id);
    }

    [[nodiscard]]
    auto find(id_t id) const noexcept -> value_type const*
    {
        auto it = m_map.find(id);
        return it != m_map.end() ? it->second.get() : nullptr;
    }

    [[nodiscard]]
    auto cached(id_t id) const noexcept -> std::shared_ptr<value_type const>
    {
        auto it = m_map.find(id);
        return it != m_map.end() ? it->second : nullptr;
    }

    [[nodiscard]]
    auto operator[](id_t id) const noexcept -> value_type const&
    {
        auto it = m_map.find(id);
        BOOST_ASSERT(it != m_map.end());
        return *it->second;
    }
    auto operator==(entity_data_map const&) const noexcept -> bool = default;

    void set(id_t id, value_type value)
    {
        auto it = m_map.find(id);
        BOOST_ASSERT(it != m_map.end());
        (*it->second) = std::move(value);
    }

    auto insert(id_t id, value_type value) -> bool
    {
        return m_map
                .insert(std::pair{
                        id,
                        std::make_shared<value_type>(std::move(value))})
                .second;
    }

    template <class... Args>
    auto emplace(id_t id, Args&&... args) -> bool
    {
        return m_map
                .emplace(
                        id,
                        std::make_shared<value_type>(
                                std::forward<Args>(args)...))
                .second;
    }

    void erase(id_t id)
    {
        m_map.erase(id);
    }

private:
    using map_t = boost::container::flat_map<id_t, std::shared_ptr<value_type>>;

    map_t m_map;
};

template <class Entity, class Data>
auto
insert(entity_data_map<entity_id<Entity>, Data>& m,
       Data&& value) -> entity_id<Entity>
{
    auto id = entity_id<Entity>::generate();
    BOOST_VERIFY(m.insert(id, std::forward<Data>(value)));
    return id;
}

} // namespace piejam
