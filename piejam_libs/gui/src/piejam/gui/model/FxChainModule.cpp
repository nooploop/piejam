// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxChainModule.h>

#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/actions/root_view_actions.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/selectors.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::gui::model
{

struct FxChainModule::Impl
{
    runtime::mixer::channel_id const fx_chain_id;
    runtime::fx::module_id const fx_mod_id;
};

FxChainModule::FxChainModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const fx_chain_id,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_chain_id, fx_mod_id))
{
}

FxChainModule::~FxChainModule() = default;

void
FxChainModule::onSubscribe()
{
    observe(runtime::selectors::make_fx_module_name_selector(m_impl->fx_mod_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::select_focused_fx_module,
            [this](auto const focused_fx_mod_id) {
                setFocused(focused_fx_mod_id == m_impl->fx_mod_id);
            });
}

void
FxChainModule::remove()
{
    runtime::actions::delete_fx_module action;
    action.fx_chain_id = m_impl->fx_chain_id;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

void
FxChainModule::focus()
{
    runtime::actions::focus_fx_module action;
    action.fx_chain_id = m_impl->fx_chain_id;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

void
FxChainModule::showFxModule()
{
    runtime::actions::show_fx_module action;
    action.fx_chain_id = m_impl->fx_chain_id;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

} // namespace piejam::gui::model
