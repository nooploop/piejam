// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/utility/utility_internal_id.h>

#include <piejam/fx_modules/module_registration.h>
#include <piejam/fx_modules/utility/gui/FxUtility.h>
#include <piejam/fx_modules/utility/utility_component.h>
#include <piejam/fx_modules/utility/utility_module.h>

namespace piejam::fx_modules::utility
{

auto
internal_id() -> runtime::fx::internal_id
{
    using namespace std::string_literals;

    static auto const id = register_module(module_registration{
            .available_for_mono = true,
            .persistence_name = "utility"s,
            .fx_module_factory = &make_module,
            .fx_component_factory = &make_component,
            .fx_browser_entry_name = "Utility",
            .fx_browser_entry_description =
                    "Amplify or attenuate an audio signal.",
            .fx_module_content_factory =
                    &piejam::gui::model::makeFxModule<gui::FxUtility>,
            .viewSource = "/PieJam/FxChainControls/ParametersListView.qml"});
    return id;
}

} // namespace piejam::fx_modules::utility
