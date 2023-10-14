// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/module_registration.h>

#include <piejam/gui/model/FxBrowserEntryInternalData.h>
#include <piejam/gui/model/FxModuleRegistry.h>
#include <piejam/gui/model/FxModuleType.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/persistence/fx_internal_id.h>

namespace piejam::fx_modules
{

auto
register_module(module_registration mod_reg) -> runtime::fx::internal_id
{
    using namespace std::string_literals;

    auto id = runtime::fx::registry::register_internal_module();

    runtime::persistence::register_internal_fx(
            id,
            std::move(mod_reg.persistence_name));

    runtime::modules::internal_fx_module_factories::add_entry(
            id,
            std::move(mod_reg.fx_module_factory));

    runtime::components::internal_fx_component_factories::add_entry(
            id,
            std::move(mod_reg.fx_component_factory));

    piejam::gui::model::FxBrowserEntryInternalDataMap::add_entry(
            id,
            {
                    .name = std::move(mod_reg.fx_browser_entry_name),
                    .description =
                            std::move(mod_reg.fx_browser_entry_description),
            });

    piejam::gui::model::FxModuleContentFactories::add_entry(
            id,
            std::move(mod_reg.fx_module_content_factory));

    piejam::gui::model::fxModuleRegistrySingleton().addItem(
            {.id = id},
            mod_reg.viewSource);

    return id;
}

} // namespace piejam::fx_modules
