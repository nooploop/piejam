// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStream.h>

#include <QObject>

namespace piejam::gui::model
{

class AudioStreamListener : public QObject
{
    Q_OBJECT

public:
    virtual void update(AudioStream const&) = 0;
};

} // namespace piejam::gui::model
