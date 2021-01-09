// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/open_alsa_device.h>

#include <piejam/audio/alsa/pcm_io.h>
#include <piejam/runtime/state.h>

namespace piejam::runtime
{

static auto
period_count(state const& state) -> unsigned
{
    constexpr unsigned default_period_count = 2u;
    unsigned min_period_count{};
    unsigned max_period_count{};

    if (state.input.index && state.output.index)
    {
        min_period_count = std::max(
                state.input.hw_params->period_count_min,
                state.output.hw_params->period_count_min);
        max_period_count = std::min(
                state.input.hw_params->period_count_max,
                state.output.hw_params->period_count_max);
    }
    else if (state.input.index)
    {
        min_period_count = state.input.hw_params->period_count_min;
        max_period_count = state.input.hw_params->period_count_max;
    }
    else
    {
        min_period_count = state.output.hw_params->period_count_min;
        max_period_count = state.output.hw_params->period_count_max;
    }

    return std::clamp(default_period_count, min_period_count, max_period_count);
}

auto
open_alsa_device(state const& state) -> std::unique_ptr<audio::device>
{
    return std::make_unique<audio::alsa::pcm_io>(
            state.pcm_devices->inputs[state.input.index],
            state.pcm_devices->outputs[state.output.index],
            audio::pcm_io_config{
                    audio::pcm_device_config{
                            state.input.hw_params->interleaved,
                            state.input.hw_params->format,
                            state.input.hw_params->num_channels},
                    audio::pcm_device_config{
                            state.output.hw_params->interleaved,
                            state.output.hw_params->format,
                            state.output.hw_params->num_channels},
                    audio::pcm_process_config{
                            state.samplerate,
                            state.period_size,
                            period_count(state)}});
}

} // namespace piejam::runtime
