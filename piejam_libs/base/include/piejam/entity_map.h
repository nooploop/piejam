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

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

#include <concepts>

namespace piejam
{

template <class EntityTag>
struct entity_id
{
    constexpr entity_id() noexcept = default;

    constexpr bool operator==(entity_id const& other) const noexcept
    {
        return m_id == other.m_id;
    }

    constexpr bool operator!=(entity_id const& other) const noexcept
    {
        return m_id != other.m_id;
    }

    constexpr bool operator<(entity_id const& other) const noexcept
    {
        return m_id < other.m_id;
    }

    static auto generate() noexcept -> entity_id<EntityTag>
    {
        static std::size_t s_id{};
        return {++s_id};
    }

private:
    constexpr entity_id(std::size_t const id) noexcept
        : m_id(id)
    {
    }

    std::size_t m_id{};
};

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

    auto contains(id_t id) const noexcept { return m_map.contains(id); }

    auto find(id_t id) const noexcept { return m_map.find(id); }
    auto find(id_t id) noexcept { return m_map.find(id); }

    auto operator[](id_t id) const noexcept -> Entity const&
    {
        BOOST_ASSERT(contains(id));
        return m_map.find(id)->second;
    }

    auto operator[](id_t id) noexcept -> Entity&
    {
        BOOST_ASSERT(contains(id));
        return m_map[id];
    }

    template <class V>
    requires std::convertible_to<V, Entity> auto add(V&& v) -> id_t
    {
        auto id = id_t::generate();
        m_map.emplace_hint(m_map.end(), id, std::forward<V>(v));
        return id;
    }

    auto remove(id_t id) { return m_map.erase(id); }

private:
    map_t m_map;
};

} // namespace piejam
