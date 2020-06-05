// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
