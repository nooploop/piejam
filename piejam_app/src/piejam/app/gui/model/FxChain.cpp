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

#include <piejam/app/gui/model/FxChain.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/model/FxModule.h>
#include <piejam/app/gui/model/FxParameter.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

FxChain::FxChain(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
FxChain::onSubscribe()
{
    observe(runtime::selectors::select_current_fx_chain,
            [this](box<runtime::fx::chain_t> const& fx_chain) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_fx_chain, *fx_chain),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *modules(),
                                [this](runtime::fx::module_id fx_mod_id) {
                                    return std::make_unique<FxModule>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            fx_mod_id);
                                }});

                m_fx_chain = fx_chain;
            });
}

} // namespace piejam::app::gui::model
