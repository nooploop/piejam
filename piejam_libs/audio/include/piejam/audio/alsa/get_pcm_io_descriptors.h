// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_descriptor.h>

namespace piejam::audio::alsa
{

auto get_pcm_io_descriptors() -> pcm_io_descriptors;

} // namespace piejam::audio::alsa
