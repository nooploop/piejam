// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>
#include <span>
#include <variant>

namespace piejam::audio
{

// from pcm to target
using pcm_input_buffer_converter = std::function<void(std::span<float>)>;

// from source to pcm
using pcm_output_source_buffer_t = std::variant<float, std::span<float const>>;
using pcm_output_buffer_converter =
        std::function<void(pcm_output_source_buffer_t const&)>;

} // namespace piejam::audio
