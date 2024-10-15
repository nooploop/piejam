// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/parameter/float_normalize.h>

#pragma once

namespace piejam::runtime::fader_mapping
{

inline constexpr auto volume = std::array{
        parameter::normalized_dB_mapping{.normalized = 0.05f, .dB = -60.f},
        parameter::normalized_dB_mapping{.normalized = 0.3f, .dB = -30.f},
        parameter::normalized_dB_mapping{.normalized = 0.55f, .dB = -12.f},
        parameter::normalized_dB_mapping{.normalized = 1.f, .dB = 6.f},
};

inline constexpr auto send = std::array{
        parameter::normalized_dB_mapping{.normalized = 0.05f, .dB = -60.f},
        parameter::normalized_dB_mapping{.normalized = 0.3f, .dB = -30.f},
        parameter::normalized_dB_mapping{.normalized = 0.48f, .dB = -18.f},
        parameter::normalized_dB_mapping{.normalized = 1.f, .dB = 0.f},
};

inline constexpr auto min_gain_dB = -160.f;

} // namespace piejam::runtime::fader_mapping
