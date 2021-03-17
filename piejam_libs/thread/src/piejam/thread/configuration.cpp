// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/configuration.h>

#include <piejam/thread/affinity.h>
#include <piejam/thread/name.h>
#include <piejam/thread/priority.h>

namespace piejam::thread
{

void
configuration::apply() const
{
    if (affinity)
        this_thread::set_affinity(*affinity);
    if (priority)
        this_thread::set_priority(*priority);
    if (name)
        this_thread::set_name(*name);
}

} // namespace piejam::thread
