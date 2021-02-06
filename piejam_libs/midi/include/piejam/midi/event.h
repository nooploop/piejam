// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/midi/device_id.h>

#include <cstddef>
#include <variant>

namespace piejam::midi
{

struct cc_event
{
    std::size_t cc{};
    std::size_t value{};
};

template <class E>
struct channel_event
{
    std::size_t channel{};
    E data{};
};

using channel_cc_event = channel_event<cc_event>;

using event_t = std::variant<channel_cc_event>;

struct external_event
{
    device_id_t device_id;
    event_t event{};
};

} // namespace piejam::midi
