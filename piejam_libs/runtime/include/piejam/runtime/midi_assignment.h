// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/midi/device_id.h>
#include <piejam/runtime/midi_assignment_id.h>

#include <map>

namespace piejam::runtime
{

struct midi_assignment
{
    std::size_t channel{};

    enum class type
    {
        cc,
    } control_type;

    std::size_t control_id{};

    constexpr auto
    operator==(midi_assignment const& other) const noexcept -> bool = default;
};

using midi_assignments_map = std::map<midi_assignment_id, midi_assignment>;

} // namespace piejam::runtime
