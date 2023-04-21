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
        constexpr auto operator()(float const x) const noexcept -> bool
        {
            return x == 0.f;
        }

        constexpr auto operator()(audio_slice::span_t const& b) const noexcept
                -> bool
        {
            return b.empty();
        }
    };

    return audio_slice::visit(is_silence_visitor{}, sl);
}

} // namespace piejam::audio::engine
