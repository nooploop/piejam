// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/priority.h>

#include <pthread.h>
#include <sched.h>

#include <system_error>

namespace piejam::this_thread
{

void
set_realtime_priority(int prio)
{
    sched_param const parm{.sched_priority = prio};
    int const status = pthread_setschedparam(pthread_self(), SCHED_FIFO, &parm);
    if (status != 0)
    {
        throw std::system_error(status, std::generic_category());
    }
}

} // namespace piejam::this_thread
