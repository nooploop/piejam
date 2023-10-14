// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/spectrum/spectrum_internal_id.h>

#include <piejam/fx_modules/module_registration.h>
#include <piejam/fx_modules/spectrum/gui/FxSpectrum.h>
#include <piejam/fx_modules/spectrum/spectrum_component.h>
#include <piejam/fx_modules/spectrum/spectrum_module.h>

namespace piejam::fx_modules::spectrum
{

auto
internal_id() -> runtime::fx::internal_id
{
    using namespace std::string_literals;

    static auto const id = register_module(module_registration{
            .persistence_name = "spectrum"s,
            .fx_module_factory = &make_module,
            .fx_component_factory = &make_component,
            .fx_browser_entry_name = "Spectrum",
            .fx_browser_entry_description =
                    "Analyze frequency content of an audio signal.",
            .fx_module_content_factory =
                    &piejam::gui::model::makeFxModuleContent<gui::FxSpectrum>,
            .viewSource = "/PieJam.FxModules/SpectrumView.qml"});
    return id;
}

} // namespace piejam::fx_modules::spectrum
