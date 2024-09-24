// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace std
{

template <class>
struct hash;

} // namespace std

namespace piejam
{

template <class EntityTag>
struct entity_id
{
    using tag = EntityTag;

    constexpr entity_id() noexcept = default;

    constexpr auto operator<=>(entity_id const& other) const noexcept = default;

    [[nodiscard]]
    constexpr auto valid() const noexcept -> bool
    {
        return m_id != 0u;
    }

    static auto generate() noexcept -> entity_id<EntityTag>
    {
        static id_t s_id{};
        return entity_id{++s_id};
    }

private:
    using id_t = std::size_t;

    constexpr explicit entity_id(id_t const id) noexcept
        : m_id(id)
    {
    }

    friend struct std::hash<entity_id<EntityTag>>;

    id_t m_id{};
};

} // namespace piejam
