// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/midi_assignment_id.h>
#include <piejam/runtime/parameters.h>

#include <map>

namespace piejam::runtime
{

struct midi_assignment
{
    enum class type
    {
        cc,
        pc
    } control_type;

    std::size_t control_id{};

    constexpr bool operator==(midi_assignment const& other) const noexcept
    {
        return control_type == other.control_type &&
               control_id == other.control_id;
    }

    constexpr bool operator!=(midi_assignment const& other) const noexcept
    {
        return !(*this == other);
    }
};

using midi_assignments_map = std::map<midi_assignment_id, midi_assignment>;

} // namespace piejam::runtime
