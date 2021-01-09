// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/store.h>

namespace piejam::runtime
{

struct store_dispatch
{
    store_dispatch(store& app_store)
        : m_store(&app_store)
    {
    }

    void operator()(runtime::action const&) const;

private:
    store* m_store;
};

} // namespace piejam::runtime
