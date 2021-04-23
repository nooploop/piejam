// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <span>

namespace piejam::gui::model
{

struct SpectrumDataPoint
{
    float frequency_Hz{};
    float level_dB{};
};

using SpectrumDataPoints = std::span<SpectrumDataPoint const>;

} // namespace piejam::gui::model
