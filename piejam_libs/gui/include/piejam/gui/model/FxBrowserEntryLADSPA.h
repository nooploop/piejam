// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/entity_id.h>
#include <piejam/gui/model/FxBrowserEntry.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::gui::model
{

class FxBrowserEntryLADSPA final : public FxBrowserEntry
{
public:
    FxBrowserEntryLADSPA(
            runtime::store_dispatch,
            runtime::subscriber&,
            audio::ladspa::plugin_descriptor const&);

    void insertModule(unsigned chainIndex, unsigned pos) override;
    void replaceModule(unsigned chainIndex, unsigned pos) override;

private:
    void onSubscribe() override;

    audio::ladspa::plugin_descriptor m_pd;
};

} // namespace piejam::gui::model
