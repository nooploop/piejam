// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxModule.h>

#include <piejam/gui/model/FxFilter.h>
#include <piejam/gui/model/FxGenericModule.h>
#include <piejam/gui/model/FxScope.h>
#include <piejam/gui/model/FxSpectrum.h>
#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/selectors.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::gui::model
{

struct FxModule::Impl
{
    std::unique_ptr<FxModuleContent> content;
};

auto
makeModuleContent(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
        -> std::unique_ptr<FxModuleContent>
{
    auto const fx_instance_id = state_change_subscriber.observe_once(
            runtime::selectors::make_fx_module_instance_id_selector(fx_mod_id));

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
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl{std::make_unique<Impl>()}
{
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
    setName(QString::fromStdString(
            *observe_once(runtime::selectors::select_focused_fx_module_name)));

    observe(runtime::selectors::select_focused_fx_module_bypassed,
            [this](bool x) { setBypassed(x); });

    observe(runtime::selectors::select_focused_fx_module,
            [this](auto fx_mod_id) {
                m_impl->content = makeModuleContent(
                        dispatch(),
                        state_change_subscriber(),
                        fx_mod_id);

                emit contentChanged();
            });
}

void
FxModule::toggleBypass()
{
    dispatch(runtime::actions::toggle_focused_fx_module_bypass{});
}

} // namespace piejam::gui::model
