// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/period_counts.h>
#include <piejam/audio/period_sizes.h>
#include <piejam/audio/samplerates.h>

#include <boost/container/static_vector.hpp>

namespace piejam::audio
{

enum class pcm_format : unsigned;

struct pcm_hw_params
{
    bool interleaved{};
    pcm_format format{};
    unsigned num_channels{};
    samplerates_t samplerates;
    period_sizes_t period_sizes;
    period_counts_t period_counts;

    bool operator==(pcm_hw_params const&) const noexcept = default;
};

} // namespace piejam::audio
