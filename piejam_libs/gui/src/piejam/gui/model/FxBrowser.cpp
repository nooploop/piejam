// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxBrowser.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/FxBrowserEntryInternal.h>
#include <piejam/gui/model/FxBrowserEntryLADSPA.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct FxBrowser::Impl
{
    runtime::fx::registry fx_registry;

    FxBrowserList entries;
};

FxBrowser::FxBrowser(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>())
{
}

FxBrowser::~FxBrowser() = default;

auto
FxBrowser::entries() noexcept -> FxBrowserList*
{
    return &m_impl->entries;
}

static auto
makeBrowserEntry(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::internal fx_type) -> std::unique_ptr<FxBrowserEntry>
{
    return std::make_unique<FxBrowserEntryInternal>(
            std::move(dispatch),
            state_change_subscriber,
            fx_type);
}

static auto
makeBrowserEntry(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber,
        ladspa::plugin_descriptor const& pd)
        -> std::unique_ptr<FxBrowserEntry>
{
    return std::make_unique<FxBrowserEntryLADSPA>(
            std::move(dispatch),
            state_change_subscriber,
            pd);
}

void
FxBrowser::onSubscribe()
{
    observe(runtime::selectors::select_fx_registry,
            [this](runtime::fx::registry const& fx_registry) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                m_impl->fx_registry.entries.get(),
                                fx_registry.entries.get()),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *entries(),
                                [this](auto const& item) {
                                    return std::visit(
                                            [this](auto&& x) {
                                                return makeBrowserEntry(
                                                        dispatch(),
                                                        state_change_subscriber(),
                                                        x);
                                            },
                                            item);
                                }});
                m_impl->fx_registry = fx_registry;
            });
}

} // namespace piejam::gui::model
