// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/midi/fwd.h>

namespace piejam::midi
{

class input_event_handler
{
public:
    virtual ~input_event_handler() = default;

    //! RT-safe
    virtual void process(event_handler&) = 0;
};

} // namespace piejam::midi
