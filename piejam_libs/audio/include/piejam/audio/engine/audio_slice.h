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

#include <piejam/audio/engine/slice.h>

namespace piejam::audio::engine
{

using audio_slice = slice<float>;

auto is_silence(audio_slice const& l) noexcept -> bool;

extern template auto
add(audio_slice const& l,
    audio_slice const& r,
    std::span<float> const& out) noexcept -> audio_slice;

extern template auto multiply(
        audio_slice const& l,
        audio_slice const& r,
        std::span<float> const& out) noexcept -> audio_slice;

extern template void
copy(audio_slice const& src, std::span<float> const& out) noexcept;

} // namespace piejam::audio::engine