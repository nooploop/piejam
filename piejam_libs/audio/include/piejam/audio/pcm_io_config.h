// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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
    unsigned samplerate{};
    unsigned period_size{};
    unsigned period_count{};
};

struct pcm_io_config
{
    pcm_device_config in_config;
    pcm_device_config out_config;
    pcm_process_config process_config;
};

} // namespace piejam::audio
