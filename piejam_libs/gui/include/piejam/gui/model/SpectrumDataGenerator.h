// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/SpectrumDataPoint.h>
#include <piejam/gui/model/Types.h>

#include <piejam/audio/fwd.h>
#include <piejam/pimpl.h>

#include <span>

namespace piejam::gui::model
{

class SpectrumDataGenerator final : public AudioStreamListener
{
    Q_OBJECT
public:
    explicit SpectrumDataGenerator(std::span<BusType const> substreamConfigs);

    void setSampleRate(audio::sample_rate);
    void setResolution(DFTResolution);

    void setActive(std::size_t substreamIndex, bool active);
    void setChannel(std::size_t substreamIndex, StereoChannel);
    void setFreeze(bool);

    void update(AudioStream) override;

signals:
    void generated(std::span<piejam::gui::model::SpectrumDataPoints const>);

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
