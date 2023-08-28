// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>

#include <vector>

namespace piejam::gui::model
{

class AudioStreamAmplifier final : public AudioStreamListener
{
    Q_OBJECT

public:
    explicit AudioStreamAmplifier(std::size_t numSubstreams);

    void update(AudioStream const&) override;

    void setGain(std::size_t substreamIndex, float gain);

signals:
    void amplified(piejam::gui::model::AudioStream);

private:
    std::vector<float> m_gains;
};

} // namespace piejam::gui::model
