// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace piejam::gui::model
{

class DbScaleTick
{
    Q_GADGET
    Q_PROPERTY(float position MEMBER position)
    Q_PROPERTY(float dB MEMBER dB)
public:
    float position{};
    float dB{};

    constexpr bool operator==(DbScaleTick const&) const noexcept = default;
};

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::DbScaleTick)
