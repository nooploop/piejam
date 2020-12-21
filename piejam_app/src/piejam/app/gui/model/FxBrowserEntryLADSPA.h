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
#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/entity_id.h>
#include <piejam/gui/model/FxBrowserEntry.h>

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
    runtime::mixer::bus_id m_fx_chain_bus;
};

} // namespace piejam::app::gui::model
