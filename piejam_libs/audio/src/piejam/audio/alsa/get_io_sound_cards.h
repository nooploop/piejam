// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sound_card_descriptor.h>

namespace piejam::audio::alsa
{

auto get_io_sound_cards() -> io_sound_cards;

} // namespace piejam::audio::alsa
