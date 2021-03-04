// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/entity_id.h>
#include <piejam/gui/model/FxBrowserEntry.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::app::gui::model
{

class FxBrowserEntryInternal final
    : public Subscribable<piejam::gui::model::FxBrowserEntry>
{
public:
    FxBrowserEntryInternal(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::internal);

    void insertModule(unsigned pos) override;
    void replaceModule(unsigned pos) override;

private:
    void onSubscribe() override;

    runtime::fx::internal m_fx_type;
    runtime::mixer::channel_id m_fx_chain_channel;
};

} // namespace piejam::app::gui::model
