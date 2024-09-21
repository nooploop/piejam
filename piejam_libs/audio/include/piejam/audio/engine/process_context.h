// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/fwd.h>

#include <functional>
#include <span>

namespace piejam::audio::engine
{

struct process_context
{
    using input_buffers_t =
            std::span<std::reference_wrapper<audio_slice const> const>;
    using output_buffers_t = std::span<std::span<float> const>;
    using result_buffers_t = std::span<audio_slice>;

    input_buffers_t inputs{};
    output_buffers_t outputs{};
    result_buffers_t results{};
    event_input_buffers const& event_inputs;
    event_output_buffers const& event_outputs;
    std::size_t buffer_size{};
};

} // namespace piejam::audio::engine
