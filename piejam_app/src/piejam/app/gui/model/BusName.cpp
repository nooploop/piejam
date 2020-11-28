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
BusName::subscribeStep(
        runtime::subscriptions_manager& subs,
        runtime::subscription_id subs_id)
{
    subs.observe(
            subs_id,
            state_change_subscriber(),
            runtime::selectors::make_bus_name_selector(m_bus_id),
            [this](container::boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });
}

} // namespace piejam::app::gui::model
