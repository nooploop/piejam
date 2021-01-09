// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>

#include <functional>

namespace std
{

 template <class EntityTag>
 struct hash<piejam::entity_id<EntityTag>>
{
    auto operator()(piejam::entity_id<EntityTag> const& e) const noexcept
            -> std::size_t
    {
        return std::hash<std::size_t>{}(e.m_id);
    }
};

} // namespace std
