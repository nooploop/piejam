// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxModule.h>

#include <piejam/gui/model/FxFilter.h>
#include <piejam/gui/model/FxGenericModule.h>
#include <piejam/gui/model/FxScope.h>
#include <piejam/gui/model/FxSpectrum.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/actions/move_fx_module.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/selectors.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::gui::model
{

struct FxModule::Impl
{
    runtime::mixer::channel_id const fx_chain_id;
    runtime::fx::module_id const fx_mod_id;

    std::unique_ptr<FxModuleContent> content;
};

auto
makeModuleContent(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id,
        runtime::fx::instance_id const fx_instance_id)
        -> std::unique_ptr<FxModuleContent>
{
    return std::visit(
            boost::hof::match(
                    [&](runtime::fx::internal fx_type)
                            -> std::unique_ptr<FxModuleContent> {
                        switch (fx_type)
                        {
                            case runtime::fx::internal::filter:
                                return std::make_unique<FxFilter>(
                                        store_dispatch,
                                        state_change_subscriber,
                                        fx_mod_id);

                            case runtime::fx::internal::scope:
                                return std::make_unique<FxScope>(
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
                    [&](auto const&) -> std::unique_ptr<FxModuleContent> {
                        return std::make_unique<FxGenericModule>(
                                store_dispatch,
                                state_change_subscriber,
                                fx_mod_id);
                    }),
            fx_instance_id);
}

FxModule::FxModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const fx_chain_id,
        runtime::fx::module_id const fx_mod_id,
        runtime::fx::instance_id const fx_instance_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              fx_chain_id,
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
FxModule::content() noexcept -> FxModuleContent*
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
                    m_impl->fx_chain_id,
                    m_impl->fx_mod_id),
            [this](bool const x) { setCanMoveLeft(x); });

    observe(runtime::selectors::make_fx_module_can_move_right_selector(
                    m_impl->fx_chain_id,
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
    action.fx_chain_id = m_impl->fx_chain_id;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

void
FxModule::moveRight()
{
    BOOST_ASSERT(canMoveRight());

    runtime::actions::move_fx_module_right action;
    action.fx_chain_id = m_impl->fx_chain_id;
    action.fx_mod_id = m_impl->fx_mod_id;
    dispatch(action);
}

} // namespace piejam::gui::model
