// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/Info.h>

namespace piejam::app::gui::model
{

class Info final : public Subscribable<piejam::gui::model::Info>
{
public:
    Info(runtime::store_dispatch, runtime::subscriber&);

private:
    void onSubscribe() override;
};

} // namespace piejam::app::gui::model
