// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/DFTResolution.h>
#include <piejam/gui/model/SpectrumDataPoint.h>
#include <piejam/gui/model/StereoChannel.h>

#include <piejam/audio/fwd.h>

#include <memory>
#include <span>

namespace piejam::gui::model
{

class SpectrumDataGenerator final : public AudioStreamListener
{
    Q_OBJECT
public:
    SpectrumDataGenerator(std::span<BusType const> substreamConfigs);
    ~SpectrumDataGenerator() override;

    void setSampleRate(audio::sample_rate);
    void setResolution(DFTResolution);

    void setActive(std::size_t substreamIndex, bool active);
    void setChannel(std::size_t substreamIndex, StereoChannel);
    void setFreeze(bool);

    void update(AudioStream const&) override;

signals:
    void generated(std::span<piejam::gui::model::SpectrumDataPoints const>);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
