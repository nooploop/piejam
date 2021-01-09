// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
