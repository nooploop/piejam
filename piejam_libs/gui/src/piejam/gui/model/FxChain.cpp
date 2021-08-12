// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxChain.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct FxChain::Impl
{
    boxed_vector<runtime::selectors::fx_module_info> fx_chain;
};

FxChain::FxChain(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>())
{
}

FxChain::~FxChain() = default;

void
FxChain::onSubscribe()
{
    observe(runtime::selectors::select_current_fx_chain,
            [this](boxed_vector<runtime::selectors::fx_module_info> const&
                           fx_chain) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->fx_chain, *fx_chain),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *modules(),
                                [this](runtime::selectors::fx_module_info const&
                                               fx_mod_info) {
                                    return std::make_unique<FxModule>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            fx_mod_info.fx_mod_id,
                                            fx_mod_info.instance_id);
                                }});

                m_impl->fx_chain = fx_chain;
            });
}

} // namespace piejam::gui::model
