// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/BusName.h>

#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

BusName::BusName(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::bus_id bus_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_bus_id(bus_id)
{
}

void
BusName::onSubscribe()
{
    observe(runtime::selectors::make_bus_name_selector(m_bus_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });
}

} // namespace piejam::app::gui::model
