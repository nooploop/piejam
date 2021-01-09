// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/audio_slice.h>

#include <piejam/audio/engine/slice.h>

namespace piejam::audio::engine
{

auto
is_silence(audio_slice const& sl) noexcept -> bool
{
    struct is_silence_visitor
    {
        constexpr bool operator()(float const x) const noexcept
        {
            return x == 0.f;
        }

        constexpr bool operator()(audio_slice::span_t const& b) const noexcept
        {
            return b.empty();
        }
    };

    return std::visit(is_silence_visitor{}, sl.as_variant());
}

} // namespace piejam::audio::engine
