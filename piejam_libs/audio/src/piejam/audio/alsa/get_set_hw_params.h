// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/system/fwd.h>

namespace piejam::audio::alsa
{

auto get_hw_params(
        sound_card_descriptor const&,
        sample_rate const*,
        period_size const*) -> sound_card_hw_params;

void set_hw_params(
        system::device&,
        sound_card_config const&,
        sound_card_buffer_config const&);

} // namespace piejam::audio::alsa
