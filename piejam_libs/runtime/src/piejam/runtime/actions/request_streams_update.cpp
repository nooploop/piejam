// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/request_streams_update.h>

#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
request_streams_update::reduce(state const& st) const -> state
{
    BOOST_ASSERT_MSG(false, "not reducible");
    return st;
}

} // namespace piejam::runtime::actions
