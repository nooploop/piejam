// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>

namespace piejam::audio
{

enum class pcm_format : unsigned;

struct sound_card_config
{
    bool interleaved{};
    pcm_format format{};
    unsigned num_channels{};
};

struct sound_card_buffer_config
{
    audio::sample_rate sample_rate;
    audio::period_size period_size;
    audio::period_count period_count;
};

struct io_process_config
{
    sound_card_config in_config;
    sound_card_config out_config;
    sound_card_buffer_config buffer_config;
};

} // namespace piejam::audio
