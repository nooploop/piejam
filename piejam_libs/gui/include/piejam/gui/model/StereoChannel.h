// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace piejam::gui::model
{

class StereoChannelClass
{
    Q_GADGET

    StereoChannelClass() noexcept = default;

public:
    enum Value
    {
        Left,
        Right,
        Middle,
        Side
    };

    Q_ENUM(Value)
};

using StereoChannel = StereoChannelClass::Value;

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::StereoChannel)
