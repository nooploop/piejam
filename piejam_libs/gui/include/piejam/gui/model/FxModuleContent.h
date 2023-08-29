// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxModuleContent : public SubscribableModel
{
    Q_OBJECT

public:
    enum class Type : int
    {
        Generic = 0,
        Filter,
        Scope,
        Spectrum
    };

    Q_ENUM(Type)

    Q_PROPERTY(Type type READ type CONSTANT FINAL)

    virtual auto type() const noexcept -> Type = 0;
};

} // namespace piejam::gui::model
