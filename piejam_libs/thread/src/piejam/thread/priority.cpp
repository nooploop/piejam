// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
