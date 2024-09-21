// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>

namespace piejam::gui::model
{

class AudioStreamChannelDuplicator final : public AudioStreamListener
{
    Q_OBJECT

public:
    void update(AudioStream const&) override;

signals:
    void duplicated(piejam::gui::model::AudioStream);
};

} // namespace piejam::gui::model
