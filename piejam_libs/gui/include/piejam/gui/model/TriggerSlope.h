// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace piejam::gui::model
{

class TriggerSlopeClass
{
    Q_GADGET

    TriggerSlopeClass() noexcept = default;

public:
    enum class Value
    {
        RisingEdge,
        FallingEdge
    };

    Q_ENUM(Value)
};

using TriggerSlope = TriggerSlopeClass::Value;

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::TriggerSlope)
