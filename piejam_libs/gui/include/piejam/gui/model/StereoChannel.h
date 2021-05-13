// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <span>

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

template <StereoChannel SC>
constexpr auto
frameValue(std::span<float const, 2> const& frame) -> float
{
    switch (SC)
    {
        case StereoChannel::Left:
            return frame[0];
        case StereoChannel::Right:
            return frame[1];
        case StereoChannel::Middle:
            return frame[0] + frame[1];
        case StereoChannel::Side:
            return frame[0] - frame[1];
    }
}

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::StereoChannel)
