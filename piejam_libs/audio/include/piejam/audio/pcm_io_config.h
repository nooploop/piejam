// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>

namespace piejam::audio
{

enum class pcm_format : unsigned;

struct pcm_device_config
{
    bool interleaved{};
    pcm_format format{};
    unsigned num_channels{};
};

struct pcm_process_config
{
    audio::sample_rate sample_rate{};
    audio::period_size period_size{};
    audio::period_count period_count{};
};

struct pcm_io_config
{
    pcm_device_config in_config;
    pcm_device_config out_config;
    pcm_process_config process_config;
};

} // namespace piejam::audio
