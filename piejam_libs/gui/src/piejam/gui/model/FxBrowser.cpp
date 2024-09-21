// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxBrowser.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/FxBrowserEntryInternal.h>
#include <piejam/gui/model/FxBrowserEntryLADSPA.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/actions/root_view_actions.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/selectors.h>

#include <ranges>

namespace piejam::gui::model
{

namespace
{

[[nodiscard]] auto
makeBrowserEntry(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::internal_id fx_type) -> std::unique_ptr<FxBrowserEntry>
{
    return std::make_unique<FxBrowserEntryInternal>(
            std::move(dispatch),
            state_change_subscriber,
            fx_type);
}

[[nodiscard]] auto
makeBrowserEntry(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber,
        ladspa::plugin_descriptor const& pd) -> std::unique_ptr<FxBrowserEntry>
{
    return std::make_unique<FxBrowserEntryLADSPA>(
            std::move(dispatch),
            state_change_subscriber,
            pd);
}

} // namespace

struct FxBrowser::Impl
{
    void updateEntries(
            runtime::store_dispatch dispatch,
            runtime::subscriber& state_change_subscriber)
    {
        std::vector<runtime::fx::registry::item> new_entries;
        std::ranges::copy(
                fx_registry.entries.get() |
                        std::views::filter(
                                runtime::fx::is_available_for_bus_type{
                                        bus_type_filter}),
                std::back_inserter(new_entries));

        algorithm::apply_edit_script(
                algorithm::edit_script(filtered_fx_registry, new_entries),
                piejam::gui::generic_list_model_edit_script_executor{
                        entries,
                        [&](auto const& item) {
                            return std::visit(
                                    [&](auto&& x) {
                                        return makeBrowserEntry(
                                                dispatch,
                                                state_change_subscriber,
                                                x);
                                    },
                                    item);
                        }});
        filtered_fx_registry = std::move(new_entries);
    }

    audio::bus_type bus_type_filter{audio::bus_type::mono};
    runtime::fx::registry fx_registry;
    std::vector<runtime::fx::registry::item> filtered_fx_registry;

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

void
FxBrowser::showMixer()
{
    runtime::actions::set_root_view_mode action;
    action.mode = runtime::root_view_mode::mixer;
    dispatch(action);
}

void
FxBrowser::onSubscribe()
{
    m_impl->bus_type_filter = observe_once(
            runtime::selectors::make_mixer_channel_bus_type_selector(
                    observe_once(
                            runtime::selectors::select_fx_browser_fx_chain)));
    m_impl->fx_registry = observe_once(runtime::selectors::select_fx_registry);
    m_impl->updateEntries(dispatch(), state_change_subscriber());
}

} // namespace piejam::gui::model
