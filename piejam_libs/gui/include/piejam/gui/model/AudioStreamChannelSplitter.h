// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/Types.h>

#include <span>
#include <vector>

namespace piejam::gui::model
{

class AudioStreamChannelSplitter final : public AudioStreamListener
{
    Q_OBJECT

public:
    explicit AudioStreamChannelSplitter(
            std::span<BusType const> substreamConfigs);

    void update(AudioStream const&) override;

signals:
    void splitted(std::size_t substreamIndex, piejam::gui::model::AudioStream);

private:
    std::vector<BusType> m_substreamConfigs;
};

} // namespace piejam::gui::model
