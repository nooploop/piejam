// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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

    constexpr bool operator==(entity_id const& other) const noexcept = default;

    constexpr bool operator<(entity_id const& other) const noexcept
    {
        return m_id < other.m_id;
    }

    constexpr explicit operator bool() const noexcept { return valid(); }
    constexpr bool valid() const noexcept { return m_id; }
    constexpr bool invalid() const noexcept { return !m_id; }

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

    friend struct std::hash<entity_id<EntityTag>>;

    id_t m_id{};
};

} // namespace piejam
