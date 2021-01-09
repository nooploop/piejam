// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>

#include <boost/container/static_vector.hpp>

#include <array>

namespace piejam::audio
{

inline constexpr period_size_t max_period_size = 1024u;
inline constexpr std::array preferred_period_sizes{
        16u,
        32u,
        64u,
        128u,
        256u,
        512u,
        max_period_size};

using period_sizes_t = boost::container::
        static_vector<period_size_t, preferred_period_sizes.size()>;

} // namespace piejam::audio
