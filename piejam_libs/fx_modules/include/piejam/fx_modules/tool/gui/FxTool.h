// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxGenericModule.h>

namespace piejam::fx_modules::tool::gui
{

class FxTool : public piejam::gui::model::FxGenericModule
{
public:
    using Base = piejam::gui::model::FxGenericModule;

    using Base::Base;

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;
};

} // namespace piejam::fx_modules::tool::gui
