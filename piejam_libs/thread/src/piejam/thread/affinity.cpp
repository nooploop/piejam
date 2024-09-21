// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/affinity.h>

#include <pthread.h>
#include <sched.h>

#include <system_error>

namespace piejam::this_thread
{

void
set_affinity(int const cpu)
{
    cpu_set_t cpuset{};
    CPU_SET(cpu, &cpuset);
    int const status =
            pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    if (status)
    {
        throw std::system_error(status, std::generic_category());
    }
}

} // namespace piejam::this_thread
