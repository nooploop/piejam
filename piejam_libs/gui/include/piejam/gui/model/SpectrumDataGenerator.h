// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SpectrumDataPoint.h>
#include <piejam/gui/model/Types.h>

#include <piejam/algorithm/shift_push_back.h>
#include <piejam/audio/fwd.h>
#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class SpectrumDataGenerator
{
public:
    explicit SpectrumDataGenerator(
            audio::sample_rate,
            DFTResolution = DFTResolution::Low);

    template <class Samples>
    auto process(Samples const& samples) -> SpectrumDataPoints
    {
        algorithm::shift_push_back(m_dftPrepareBuffer, samples);
        return process();
    }

private:
    auto process() -> SpectrumDataPoints;

    struct Impl;
    pimpl<Impl> m_impl;

    std::vector<float> m_dftPrepareBuffer;
};

} // namespace piejam::gui::model
