// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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
    unsigned period_count_min{};
    unsigned period_count_max{};
};

inline bool
operator==(pcm_hw_params const& l, pcm_hw_params const& r) noexcept
{
    return l.interleaved == r.interleaved && l.format == r.format &&
           l.num_channels == r.num_channels && l.samplerates == r.samplerates &&
           l.period_sizes == r.period_sizes &&
           l.period_count_min == r.period_count_min &&
           l.period_count_max == r.period_count_max;
}

inline bool
operator!=(pcm_hw_params const& l, pcm_hw_params const& r) noexcept
{
    return !(l == r);
}

} // namespace piejam::audio
