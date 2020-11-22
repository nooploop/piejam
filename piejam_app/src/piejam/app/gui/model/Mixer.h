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
#include <piejam/gui/model/Mixer.h>
#include <piejam/runtime/subscriber.h>

namespace piejam::app::gui::model
{

class Mixer final
    : public piejam::gui::model::Mixer
    , public Subscribable
{
    Q_OBJECT

    Q_PROPERTY(bool subscribed READ subscribed WRITE setSubscribed NOTIFY
                       subscribedChanged)
public:
    Mixer(runtime::store_dispatch, runtime::subscriber&);

    void setInputSolo(unsigned index) override;
    void requestLevelsUpdate() override;

signals:
    void subscribedChanged();

private:
    void subscribeStep(
            runtime::subscriber&,
            runtime::subscriptions_manager&,
            runtime::subscription_id) override;

    void emitSubscribedChangedSignal() override { emit subscribedChanged(); }

    runtime::mixer::bus_list_t m_inputs;
    runtime::mixer::bus_list_t m_outputs;
    runtime::mixer::bus_list_t m_all;
};

} // namespace piejam::app::gui::model
