// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/tool/gui/FxTool.h>

#include <piejam/fx_modules/tool/tool_internal_id.h>

namespace piejam::fx_modules::tool::gui
{

using namespace piejam::gui::model;

auto
FxTool::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

} // namespace piejam::fx_modules::tool::gui
