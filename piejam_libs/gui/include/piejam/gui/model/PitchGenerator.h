// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/shift_push_back.h>
#include <piejam/audio/sample_rate.h>

#include <vector>

namespace piejam::gui::model
{

class PitchGenerator
{
public:
    explicit PitchGenerator(audio::sample_rate);

    template <class Samples>
    auto process(Samples const& samples) -> float
    {
        algorithm::shift_push_back(m_signal, samples);
        m_captured_samples += std::ranges::size(samples);
        return process();
    }

private:
    auto process() -> float;

    std::vector<float> m_signal;
    std::size_t m_captured_samples{};
    float m_last_frequency{};
    audio::sample_rate m_sample_rate;
};

} // namespace piejam::gui::model
