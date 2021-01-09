// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/BusName.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::app::gui::model
{

class BusName final : public Subscribable<piejam::gui::model::BusName>
{
public:
    BusName(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::bus_id);

private:
    void onSubscribe() override;

    runtime::mixer::bus_id m_bus_id;
};

} // namespace piejam::app::gui::model
