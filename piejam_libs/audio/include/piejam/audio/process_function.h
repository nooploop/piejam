// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_buffer_converter.h>

#include <functional>
#include <span>

namespace piejam::audio
{

using process_function = std::function<void(
        std::span<pcm_input_buffer_converter const> const& /*in*/,
        std::span<pcm_output_buffer_converter const> const& /*out*/,
        std::size_t /*buffer_size*/)>;

} // namespace piejam::audio
