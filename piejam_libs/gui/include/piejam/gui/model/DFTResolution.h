// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace piejam::gui::model
{

class DFTResolutionClass
{
    Q_GADGET

    DFTResolutionClass() noexcept = default;

public:
    enum class Value
    {
        Low,
        Middle,
        High,
        VeryHigh,
    };

    Q_ENUM(Value)
};

using DFTResolution = DFTResolutionClass::Value;

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::DFTResolution)
