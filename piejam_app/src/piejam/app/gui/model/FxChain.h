// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/box.h>
#include <piejam/gui/model/FxChain.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::app::gui::model
{

class FxChain final : public Subscribable<piejam::gui::model::FxChain>
{
public:
    FxChain(runtime::store_dispatch, runtime::subscriber&);

private:
    void onSubscribe() override;

    box<runtime::fx::chain_t> m_fx_chain;
};

} // namespace piejam::app::gui::model
