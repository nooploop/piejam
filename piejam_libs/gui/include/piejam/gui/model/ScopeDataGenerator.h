// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/ScopeData.h>
#include <piejam/gui/model/Types.h>

#include <piejam/audio/fwd.h>
#include <piejam/pimpl.h>

#include <chrono>

namespace piejam::gui::model
{

class ScopeDataGenerator final : public AudioStreamListener
{
    Q_OBJECT
public:
    explicit ScopeDataGenerator(std::span<BusType const> substreamConfigs);

    void setSampleRate(audio::sample_rate);
    void setResolution(std::size_t resolution); // 1 .. 4
    void setWindowSize(std::size_t numSamples);
    void setHoldTime(std::chrono::milliseconds);
    void setTriggerStream(std::size_t substreamIndex);
    void setTriggerSlope(TriggerSlope);
    void setTriggerLevel(float);
    void setActive(std::size_t substreamIndex, bool active);
    void setChannel(std::size_t substreamIndex, StereoChannel);
    void setFreeze(bool);

    void update(AudioStream) override;

    void clear();

signals:
    void generated(std::span<ScopeData::Samples const>);

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
