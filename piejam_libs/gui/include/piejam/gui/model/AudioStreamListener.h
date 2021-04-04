// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <span>

namespace piejam::gui::model
{

class AudioStreamListener : public QObject
{
    Q_OBJECT

public:
    virtual void update(std::span<float const> const&) = 0;
};

} // namespace piejam::gui::model
