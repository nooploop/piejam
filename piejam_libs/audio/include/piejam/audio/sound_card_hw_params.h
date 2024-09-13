// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>

namespace piejam::audio
{

enum class pcm_format : unsigned;

struct sound_card_hw_params
{
    bool interleaved{};
    pcm_format format{};
    unsigned num_channels{};
    sample_rates_t sample_rates;
    period_sizes_t period_sizes;
    period_counts_t period_counts;

    auto operator==(sound_card_hw_params const&) const noexcept
            -> bool = default;
};

} // namespace piejam::audio
