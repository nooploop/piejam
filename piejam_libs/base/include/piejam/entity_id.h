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

#include <cstddef>

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
        static id_t s_id{};
        return {++s_id};
    }

private:

    using id_t = std::size_t;

    constexpr entity_id(id_t const id) noexcept
        : m_id(id)
    {
    }

    id_t m_id{};
};

} // namespace piejam
