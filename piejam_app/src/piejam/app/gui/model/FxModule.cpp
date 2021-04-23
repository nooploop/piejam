// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxModule.h>

#include <piejam/app/gui/model/FxGenericModule.h>
#include <piejam/app/gui/model/FxScopeModule.h>
#include <piejam/app/gui/model/FxSpectrum.h>
#include <piejam/functional/overload.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/actions/move_fx_module.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/selectors.h>

#include <boost/assert.hpp>

namespace piejam::app::gui::model
{

struct FxModule::Impl
{
    runtime::fx::module_id const fx_mod_id;

    std::unique_ptr<piejam::gui::model::FxModuleContent> content;
};

auto
makeModuleContent(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id,
        runtime::fx::instance_id const fx_instance_id)
        -> std::unique_ptr<piejam::gui::model::FxModuleContent>
{
    return std::visit(
            overload{
                    [&](runtime::fx::internal fx_type)
                            -> std::unique_ptr<
                                    piejam::gui::model::FxModuleContent> {
                        switch (fx_type)
                        {
                            case runtime::fx::internal::scope:
                                return std::make_unique<FxScopeModule>(
                                        store_dispatch,
                                        state_change_subscriber,
                                        fx_mod_id);

                            case runtime::fx::internal::spectrum:
                                return std::make_unique<FxSpectrum>(
                                        store_dispatch,
                                        state_change_subscriber,
                                        fx_mod_id);

                            default:
                                return std::make_unique<FxGenericModule>(
                                        store_dispatch,
                                        state_change_subscriber,
                                        fx_mod_id);
                        }
                    },
                    [&](auto const&)
                            -> std::unique_ptr<
                                    piejam::gui::model::FxModuleContent> {
                        return std::make_unique<FxGenericModule>(
                                store_dispatch,
                                state_change_subscriber,
                                fx_mod_id);
                    }},
            fx_instance_id);
}

FxModule::FxModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id,
        runtime::fx::instance_id const fx_instance_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              fx_mod_id,
              makeModuleContent(
                      store_dispatch,
                      state_change_subscriber,
                      fx_mod_id,
                      fx_instance_id)))
{
    connect(*m_impl->content);
}

FxModule::~FxModule() = default;

auto
FxModule::content() noexcept -> piejam::gui::model::FxModuleContent*
{
    return m_impl->content.get();
}

void
FxModule::onSubscribe()
{
    observe(runtime::selectors::make_fx_module_name_selector(m_impl->fx_mod_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_fx_module_bypass_selector(
                    m_impl->fx_mod_id),
            [this](bool const x) { setBypassed(x); });

    observe(runtime::selectors::make_fx_module_can_move_left_selector(
                    m_impl->fx_mod_id),
            [this](bool const x) { setCanMoveLeft(x); });

    observe(runtime::selectors::make_fx_module_can_move_right_selector(
                    m_impl->fx_mod_id),
            [this](bool const x) { setCanMoveRight(x); });
}

void
FxModule::toggleBypass()
{
    runtime::actions::toggle_fx_module_bypass action;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

void
FxModule::deleteModule()
{
    runtime::actions::delete_fx_module action;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

void
FxModule::moveLeft()
{
    BOOST_ASSERT(canMoveLeft());

    runtime::actions::move_fx_module_left action;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

void
FxModule::moveRight()
{
    BOOST_ASSERT(canMoveRight());

    runtime::actions::move_fx_module_right action;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

} // namespace piejam::app::gui::model
