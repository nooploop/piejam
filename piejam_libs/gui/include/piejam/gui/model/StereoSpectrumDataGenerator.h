// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/SpectrumDataPoint.h>
#include <piejam/gui/model/StereoChannel.h>

#include <memory>

namespace piejam::gui::model
{

class StereoSpectrumDataGenerator final : public AudioStreamListener
{
    Q_OBJECT
public:
    StereoSpectrumDataGenerator();
    ~StereoSpectrumDataGenerator();

    void setSampleRate(audio::sample_rate const&);
    void setActive(bool active);
    void setChannel(StereoChannel);

    void update(Stream const&) override;

signals:
    void generated(SpectrumDataPoints const&);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
