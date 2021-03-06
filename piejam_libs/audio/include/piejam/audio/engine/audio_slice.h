// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::audio::engine
{

template <class T>
class slice;

using audio_slice = slice<float>;

auto is_silence(audio_slice const& l) noexcept -> bool;

} // namespace piejam::audio::engine
