// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/PitchGenerator.h>

#include <piejam/audio/dsp/pitch_yin.h>
#include <piejam/audio/dsp/rms.h>

namespace piejam::gui::model
{

namespace
{

inline constexpr std::size_t windowSize{8192};
inline constexpr std::size_t captureSize{4096};

} // namespace

PitchGenerator::PitchGenerator(audio::sample_rate sample_rate)
    : m_signal(windowSize)
    , m_sample_rate{sample_rate}
{
}

auto
PitchGenerator::process() -> float
{
    if (m_captured_samples >= captureSize)
    {
        if (audio::dsp::simd::rms<float>(m_signal) < 0.001) // -60 dB
        {
            m_last_frequency = 0.f;
        }
        else
        {
            m_last_frequency =
                    audio::dsp::pitch_yin<float>(m_signal, m_sample_rate);
        }

        m_captured_samples %= captureSize;
    }

    return m_last_frequency;
}

} // namespace piejam::gui::model
