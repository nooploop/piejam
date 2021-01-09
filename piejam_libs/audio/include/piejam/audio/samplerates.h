// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>

#include <boost/container/static_vector.hpp>

#include <array>

namespace piejam::audio
{

inline constexpr std::array preferred_samplerates{
        11025u,
        12000u,
        22050u,
        24000u,
        44100u,
        48000u,
        88200u,
        96000u,
        176400u,
        192000u};

using samplerates_t = boost::container::
        static_vector<samplerate_t, preferred_samplerates.size()>;

} // namespace piejam::audio
