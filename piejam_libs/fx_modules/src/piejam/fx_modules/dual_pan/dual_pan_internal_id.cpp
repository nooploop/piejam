// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/dual_pan/dual_pan_internal_id.h>

#include <piejam/fx_modules/dual_pan/dual_pan_component.h>
#include <piejam/fx_modules/dual_pan/dual_pan_module.h>
#include <piejam/fx_modules/dual_pan/gui/FxDualPan.h>
#include <piejam/fx_modules/module_registration.h>

namespace piejam::fx_modules::dual_pan
{

auto
internal_id() -> runtime::fx::internal_id
{
    using namespace std::string_literals;

    static auto const id = register_module(module_registration{
            .available_for_mono = false,
            .persistence_name = "dual_pan"s,
            .fx_module_factory = &make_module,
            .fx_component_factory = &make_component,
            .fx_browser_entry_name = "Dual Pan",
            .fx_browser_entry_description =
                    "Pan left and right channel separately.",
            .fx_module_content_factory =
                    &piejam::gui::model::makeFxModule<gui::FxDualPan>,
            .viewSource = "/PieJam/FxChainControls/ParametersListView.qml"});
    return id;
}

} // namespace piejam::fx_modules::dual_pan
