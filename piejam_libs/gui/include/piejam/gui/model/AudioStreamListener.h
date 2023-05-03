// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>

#include <QObject>

namespace piejam::gui::model
{

class AudioStreamListener : public QObject
{
    Q_OBJECT

public:
    using Stream = audio::dynamic_interleaved_view<float const>;

    virtual void update(Stream const&) = 0;
};

} // namespace piejam::gui::model
