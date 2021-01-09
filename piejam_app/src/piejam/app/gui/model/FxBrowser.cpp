// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxBrowser.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/model/FxBrowserEntryInternal.h>
#include <piejam/app/gui/model/FxBrowserEntryLADSPA.h>
#include <piejam/functional/overload.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
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
FxBrowser::onSubscribe()
{
    observe(runtime::selectors::select_fx_registry,
            [this](runtime::fx::registry const& fx_registry) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                m_fx_registry.entries.get(),
                                fx_registry.entries.get()),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *entries(),
                                [this](auto const& item) {
                                    return std::visit(
                                            [this](auto&& x) {
                                                return makeBrowserEntry(x);
                                            },
                                            item);
                                }});
                m_fx_registry = fx_registry;
            });
}

auto
FxBrowser::makeBrowserEntry(runtime::fx::internal const fx_type)
        -> std::unique_ptr<piejam::gui::model::FxBrowserEntry>
{
    return std::make_unique<FxBrowserEntryInternal>(
            dispatch(),
            state_change_subscriber(),
            fx_type);
}

auto
FxBrowser::makeBrowserEntry(audio::ladspa::plugin_descriptor const& pd)
        -> std::unique_ptr<piejam::gui::model::FxBrowserEntry>
{
    return std::make_unique<FxBrowserEntryLADSPA>(
            dispatch(),
            state_change_subscriber(),
            pd);
}

} // namespace piejam::app::gui::model
