// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/filter/filter_internal_id.h>

#include <piejam/fx_modules/filter/filter_component.h>
#include <piejam/fx_modules/filter/filter_module.h>
#include <piejam/fx_modules/filter/gui/FxFilter.h>
#include <piejam/fx_modules/module_registration.h>

namespace piejam::fx_modules::filter
{

auto
internal_id() -> runtime::fx::internal_id
{
    using namespace std::string_literals;

    static auto const id = register_module(module_registration{
            .available_for_mono = true,
            .persistence_name = "filter"s,
            .fx_module_factory = &make_module,
            .fx_component_factory = &make_component,
            .fx_browser_entry_name = "Filter",
            .fx_browser_entry_description = "Filter an audio signal.",
            .fx_module_content_factory =
                    &piejam::gui::model::makeFxModuleContent<gui::FxFilter>,
            .viewSource = "/PieJam.FxModules/FilterView.qml"});
    return id;
}

} // namespace piejam::fx_modules::filter
