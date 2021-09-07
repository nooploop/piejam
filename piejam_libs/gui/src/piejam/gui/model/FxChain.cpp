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
    runtime::mixer::channel_id fx_chain_id;
    boxed_vector<runtime::selectors::fx_module_info> fx_chain;

    FxModulesList modules;
};

FxChain::FxChain(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id fx_chain_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_chain_id))
{
}

FxChain::~FxChain() = default;

auto
FxChain::modules() noexcept -> FxModulesList*
{
    return &m_impl->modules;
}

void
FxChain::onSubscribe()
{
    observe(runtime::selectors::make_fx_chain_selector(m_impl->fx_chain_id),
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
                                            m_impl->fx_chain_id,
                                            fx_mod_info.fx_mod_id,
                                            fx_mod_info.instance_id);
                                }});

                m_impl->fx_chain = fx_chain;
            });
}

} // namespace piejam::gui::model
