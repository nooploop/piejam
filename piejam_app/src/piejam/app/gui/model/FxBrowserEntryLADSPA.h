// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/entity_id.h>
#include <piejam/gui/model/FxBrowserEntry.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::app::gui::model
{

class FxBrowserEntryLADSPA final
    : public Subscribable<piejam::gui::model::FxBrowserEntry>
{
public:
    FxBrowserEntryLADSPA(
            runtime::store_dispatch,
            runtime::subscriber&,
            audio::ladspa::plugin_descriptor const&);

    void insertModule(unsigned pos) override;
    void replaceModule(unsigned pos) override;

private:
    void onSubscribe() override;

    audio::ladspa::plugin_descriptor m_pd;
    runtime::mixer::channel_id m_fx_chain_bus;
};

} // namespace piejam::app::gui::model
