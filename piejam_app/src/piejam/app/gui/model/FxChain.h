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

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/container/box.h>
#include <piejam/gui/model/FxChain.h>
#include <piejam/runtime/subscriber.h>

namespace piejam::app::gui::model
{

class FxChain final : public Subscribable<piejam::gui::model::FxChain>
{
public:
    FxChain(runtime::store_dispatch, runtime::subscriber&);

    void selectBus(int) override;

private:
    void subscribeStep(
            runtime::subscriber&,
            runtime::subscriptions_manager&,
            runtime::subscription_id) override;

    void updateBuses(
            audio::bus_direction,
            container::box<runtime::mixer::bus_list_t> const&,
            runtime::subscriber&);

    runtime::mixer::bus_list_t m_inputs;
    runtime::mixer::bus_list_t m_outputs;
    runtime::mixer::bus_list_t m_all;
};

} // namespace piejam::app::gui::model
