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

#include <piejam/app/gui/model/FxBrowser.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/generic_list_model_edit_script_executor.h>
#include <piejam/app/gui/model/FxBrowserEntry.h>
#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

FxBrowser::FxBrowser(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
FxBrowser::subscribe_step()
{
    observe(runtime::selectors::select_fx_registry,
            [this](runtime::fx::registry const& fx_registry) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                m_fx_registry.entries,
                                fx_registry.entries),
                        generic_list_model_edit_script_executor{
                                *entries(),
                                [this](runtime::fx::registry::item const&
                                               item) {
                                    return std::make_unique<FxBrowserEntry>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            item);
                                }});
                m_fx_registry = fx_registry;
            });
}

} // namespace piejam::app::gui::model
