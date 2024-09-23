// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/tuner/tuner_internal_id.h>

#include <piejam/fx_modules/module_registration.h>
#include <piejam/fx_modules/tuner/gui/FxTuner.h>
#include <piejam/fx_modules/tuner/tuner_component.h>
#include <piejam/fx_modules/tuner/tuner_module.h>

namespace piejam::fx_modules::tuner
{

auto
internal_id() -> runtime::fx::internal_id
{
    using namespace std::string_literals;

    static auto const id = register_module(module_registration{
            .available_for_mono = true,
            .persistence_name = "tuner"s,
            .fx_module_factory = &make_module,
            .fx_component_factory = &make_component,
            .fx_browser_entry_name = "Tuner",
            .fx_browser_entry_description = "Detect pitch of an audio signal.",
            .fx_module_content_factory =
                    &piejam::gui::model::makeFxModule<gui::FxTuner>,
            .viewSource = "/PieJam.FxModules/TunerView.qml"});
    return id;
}

} // namespace piejam::fx_modules::tuner
