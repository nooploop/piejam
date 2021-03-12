// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>
#include <span>

namespace piejam::audio
{

using pcm_input_buffer_converter = std::function<void(std::span<float> const&)>;
using pcm_output_buffer_converter =
        std::function<void(std::span<float const> const&)>;

} // namespace piejam::audio
