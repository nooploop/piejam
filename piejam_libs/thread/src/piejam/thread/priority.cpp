// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/priority.h>

#include <cstring>
#include <stdexcept>
#include <thread>

namespace piejam::this_thread
{

void
set_priority(int prio)
{
    sched_param const parm{prio};
    int const status = pthread_setschedparam(pthread_self(), SCHED_FIFO, &parm);
    if (status)
        throw std::runtime_error(std::strerror(status));
}

} // namespace piejam::this_thread
