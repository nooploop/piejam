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

template <class Data>
using cached_entity_data_ptr = std::shared_ptr<Data const>;

template <class Id, class Data>
class entity_data_map
{
public:
    using id_type = Id;
    using data_type = Data;

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
    auto begin() const noexcept
    {
        return m_map.begin();
    }

    [[nodiscard]]
    auto end() const noexcept
    {
        return m_map.end();
    }

    [[nodiscard]]
    auto contains(id_type id) const noexcept
    {
        return m_map.contains(id);
    }

    [[nodiscard]]
    auto find(id_type id) const noexcept -> data_type const*
    {
        auto it = m_map.find(id);
        return it != m_map.end() ? it->second.get() : nullptr;
    }

    [[nodiscard]]
    auto cached(id_type id) const noexcept -> cached_entity_data_ptr<data_type>
    {
        auto it = m_map.find(id);
        return it != m_map.end() ? it->second : nullptr;
    }

    [[nodiscard]]
    auto operator[](id_type id) const noexcept -> data_type const&
    {
        auto it = m_map.find(id);
        BOOST_ASSERT(it != m_map.end());
        return *it->second;
    }
    auto operator==(entity_data_map const&) const noexcept -> bool = default;

    void set(id_type id, data_type value)
    {
        auto it = m_map.find(id);
        BOOST_ASSERT(it != m_map.end());
        (*it->second) = std::move(value);
    }

    auto insert(id_type id, data_type value) -> bool
    {
        return m_map
                .insert(std::pair{
                        id,
                        std::make_shared<data_type>(std::move(value))})
                .second;
    }

    template <class... Args>
    auto emplace(id_type id, Args&&... args) -> bool
    {
        return m_map
                .emplace(
                        id,
                        std::make_shared<data_type>(
                                std::forward<Args>(args)...))
                .second;
    }

    void erase(id_type id)
    {
        m_map.erase(id);
    }

private:
    using map_t =
            boost::container::flat_map<id_type, std::shared_ptr<data_type>>;

    map_t m_map;
};

template <class Entity, class Data>
auto
insert(entity_data_map<entity_id<Entity>, std::decay_t<Data>>& m,
       Data&& value) -> entity_id<Entity>
{
    auto id = entity_id<Entity>::generate();
    BOOST_VERIFY(m.insert(id, std::forward<Data>(value)));
    return id;
}

} // namespace piejam
