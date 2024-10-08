// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxModuleView.h>

#include <piejam/gui/model/FxGenericModule.h>
#include <piejam/gui/model/FxModuleFactory.h>

#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/selectors.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::gui::model
{

struct FxModuleView::Impl
{
    runtime::fx::module_id fx_mod_id;
    std::unique_ptr<FxModule> content;
};

auto
makeModuleContent(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id) -> std::unique_ptr<FxModule>
{
    auto const fx_instance_id = state_change_subscriber.observe_once(
            runtime::selectors::make_fx_module_instance_id_selector(fx_mod_id));

    return std::visit(
            boost::hof::match(
                    [&](runtime::fx::internal_id fx_type)
                            -> std::unique_ptr<FxModule> {
                        return FxModuleFactories::lookup(fx_type)(
                                store_dispatch,
                                state_change_subscriber,
                                fx_mod_id);
                    },
                    [&](auto const&) -> std::unique_ptr<FxModule> {
                        return std::make_unique<FxGenericModule>(
                                store_dispatch,
                                state_change_subscriber,
                                fx_mod_id);
                    }),
            fx_instance_id);
}

FxModuleView::FxModuleView(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl{make_pimpl<Impl>()}
{
}

auto
FxModuleView::content() noexcept -> FxModule*
{
    return m_impl->content.get();
}

void
FxModuleView::onSubscribe()
{
    setColor(static_cast<MaterialColor>(
            observe_once(runtime::selectors::select_focused_fx_module_color)));

    setChainName(QString::fromStdString(observe_once(
            runtime::selectors::make_mixer_channel_name_selector(observe_once(
                    runtime::selectors::select_focused_fx_chain)))));

    setName(QString::fromStdString(
            *observe_once(runtime::selectors::select_focused_fx_module_name)));

    observe(runtime::selectors::select_focused_fx_module_bypassed,
            [this](bool x) { setBypassed(x); });

    observe(runtime::selectors::select_focused_fx_module,
            [this](runtime::fx::module_id const fx_mod_id) {
                if (m_impl->fx_mod_id != fx_mod_id)
                {
                    m_impl->fx_mod_id = fx_mod_id;

                    auto content = makeModuleContent(
                            dispatch(),
                            state_change_subscriber(),
                            fx_mod_id);

                    std::swap(m_impl->content, content);

                    emit contentChanged();
                }
            });
}

void
FxModuleView::toggleBypass()
{
    dispatch(runtime::actions::toggle_focused_fx_module_bypass{});
}

} // namespace piejam::gui::model
