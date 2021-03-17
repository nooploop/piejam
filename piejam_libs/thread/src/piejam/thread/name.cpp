// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/name.h>

#include <boost/assert.hpp>

#include <pthread.h>

#include <system_error>

namespace piejam::this_thread
{

void
set_name(std::string_view const& name)
{
    BOOST_ASSERT(name.length() < 16);
    int const status = pthread_setname_np(pthread_self(), name.data());
    if (status)
        throw std::system_error(status, std::generic_category());
}

} // namespace piejam::this_thread
