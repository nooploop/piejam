// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleType.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::gui::model
{

class FxModuleContent : public SubscribableModel
{
    Q_OBJECT

public:
    Q_PROPERTY(piejam::gui::model::FxModuleType type READ type CONSTANT FINAL)

    virtual auto type() const noexcept -> FxModuleType = 0;
};

} // namespace piejam::gui::model
