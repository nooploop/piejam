// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/midi/fwd.h>

namespace piejam::midi
{

class event_handler
{
public:
    virtual ~event_handler() = default;

    virtual void process(external_event const&) = 0;
};

} // namespace piejam::midi
