// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>

#include <boost/container/static_vector.hpp>

#include <array>

namespace piejam::audio
{

inline constexpr std::array preferred_period_counts{2u, 3u, 4u, 5u, 6u};

using period_counts_t = boost::container::
        static_vector<period_size_t, preferred_period_counts.size()>;

} // namespace piejam::audio
