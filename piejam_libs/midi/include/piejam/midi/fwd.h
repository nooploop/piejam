// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <variant>

namespace piejam::midi
{

struct device_added;
struct device_removed;
using device_update = std::variant<device_added, device_removed>;

class event_handler;
class input_event_handler;
class device_manager;

struct cc_event;

template <class E>
struct channel_event;

using channel_cc_event = channel_event<cc_event>;

using event_t = std::variant<channel_cc_event>;

struct external_event;

} // namespace piejam::midi
