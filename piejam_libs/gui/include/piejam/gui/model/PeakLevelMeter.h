// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/AudioStreamListener.h>

#include <piejam/audio/sample_rate.h>
#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class PeakLevelMeter final : public AudioStreamListener
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(double, level, setLevel)

public:
    PeakLevelMeter(audio::sample_rate);

    void setSampleRate(audio::sample_rate);

    void update(AudioStream const&) override;

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
