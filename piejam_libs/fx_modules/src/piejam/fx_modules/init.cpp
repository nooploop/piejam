// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/dual_pan/dual_pan_internal_id.h>
#include <piejam/fx_modules/dual_pan/gui/FxDualPan.h>
#include <piejam/fx_modules/filter/filter_internal_id.h>
#include <piejam/fx_modules/filter/gui/FxFilter.h>
#include <piejam/fx_modules/scope/gui/FxScope.h>
#include <piejam/fx_modules/scope/scope_internal_id.h>
#include <piejam/fx_modules/spectrum/gui/FxSpectrum.h>
#include <piejam/fx_modules/spectrum/spectrum_internal_id.h>
#include <piejam/fx_modules/tool/gui/FxTool.h>
#include <piejam/fx_modules/tool/tool_internal_id.h>

#include <QDir>
#include <QtQml>

#include <mutex>

static void
initResources()
{
    Q_INIT_RESOURCE(piejam_fx_modules_resources);

    qRegisterMetaType<piejam::fx_modules::dual_pan::gui::FxDualPan*>();
    qRegisterMetaType<piejam::fx_modules::filter::gui::FxFilter*>();
    qRegisterMetaType<piejam::fx_modules::scope::gui::FxScope*>();
    qRegisterMetaType<piejam::fx_modules::spectrum::gui::FxSpectrum*>();
    qRegisterMetaType<piejam::fx_modules::tool::gui::FxTool*>();

    qmlRegisterUncreatableType<piejam::fx_modules::scope::gui::FxScope>(
            "PieJam.FxModules.Models",
            1,
            0,
            "FxScope",
            "Not createable");
}

namespace piejam::fx_modules
{

void
init()
{
    static std::once_flag s_init;
    std::call_once(s_init, []() {
        initResources();

        dual_pan::internal_id();
        filter::internal_id();
        scope::internal_id();
        spectrum::internal_id();
        tool::internal_id();
    });
}

} // namespace piejam::fx_modules
