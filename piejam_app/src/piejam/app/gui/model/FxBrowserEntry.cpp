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

#include <piejam/app/gui/model/FxBrowserEntry.h>

#include <piejam/functional/overload.h>
#include <piejam/runtime/actions/add_fx_module.h>
#include <piejam/runtime/selectors.h>

#include <fmt/format.h>

namespace piejam::app::gui::model
{

FxBrowserEntry::FxBrowserEntry(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::registry::item registry_item)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_registry_item(registry_item)
{
    static QString s_section_internal{tr("Internal")};
    static QString s_section_ladspa{tr("LADSPA Plugin")};

    std::visit(
            overload{
                    [this](runtime::fx::type fx_type) {
                        switch (fx_type)
                        {
                            case runtime::fx::type::gain:
                                setName(tr("Gain"));
                                setSection(s_section_internal);
                                setDescription(
                                        tr("Amplify or attenuate a signal."));
                                break;
                        }
                    },
                    [this](audio::ladspa::plugin_descriptor const& pd) {
                        setName(QString::fromStdString(pd.name));
                        setSection(s_section_ladspa);
                        setDescription(QString::fromStdString(fmt::format(
                                "Author: {}, Copyright: {}",
                                pd.author,
                                pd.copyright)));
                    }},
            m_registry_item);
}

void
FxBrowserEntry::subscribe_step()
{
}

void
FxBrowserEntry::addModule()
{
    runtime::actions::add_fx_module action;
    action.reg_item = m_registry_item;
    dispatch(action);
}

} // namespace piejam::app::gui::model
