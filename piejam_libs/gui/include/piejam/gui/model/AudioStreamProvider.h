// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStream.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <QPointer>

namespace piejam::gui::model
{

class AudioStreamProvider : public SubscribableModel
{
    Q_OBJECT

signals:
    void captured(piejam::gui::model::AudioStream);
};

} // namespace piejam::gui::model
