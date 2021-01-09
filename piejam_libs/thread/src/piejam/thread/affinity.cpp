// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/affinity.h>

#include <cstring>
#include <stdexcept>
#include <thread>

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
        throw std::runtime_error(std::strerror(status));
}

} // namespace piejam::this_thread
