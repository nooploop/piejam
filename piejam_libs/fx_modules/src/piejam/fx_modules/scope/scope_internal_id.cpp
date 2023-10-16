// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/scope/scope_internal_id.h>

#include <piejam/fx_modules/module_registration.h>
#include <piejam/fx_modules/scope/gui/FxScope.h>
#include <piejam/fx_modules/scope/scope_component.h>
#include <piejam/fx_modules/scope/scope_module.h>

namespace piejam::fx_modules::scope
{

auto
internal_id() -> runtime::fx::internal_id
{
    using namespace std::string_literals;

    static auto const s_id = register_module(module_registration{
            .available_for_mono = true,
            .persistence_name = "scope"s,
            .fx_module_factory = &make_module,
            .fx_component_factory = &make_component,
            .fx_browser_entry_name = "Oscilloscope",
            .fx_browser_entry_description =
                    "Observe waveform of an audio signal.",
            .fx_module_content_factory =
                    &piejam::gui::model::makeFxModuleContent<gui::FxScope>,
            .viewSource = "/PieJam.FxModules/ScopeView.qml"});

    return s_id;
}

} // namespace piejam::fx_modules::scope
