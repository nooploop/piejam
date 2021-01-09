// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/store_dispatch.h>

#include <piejam/redux/store.h>
#include <piejam/runtime/state.h>

namespace piejam::runtime
{

void
store_dispatch::operator()(action const& a) const
{
    m_store->dispatch(a);
}

} // namespace piejam::runtime
