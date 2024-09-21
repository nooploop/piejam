// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_buffer_converter.h>

#include <span>
#include <system_error>

namespace piejam::audio::alsa
{

class pcm_writer
{
public:
    using converter_f = pcm_output_buffer_converter;

    virtual ~pcm_writer() = default;

    [[nodiscard]] virtual auto
    converter() const noexcept -> std::span<converter_f const> = 0;

    [[nodiscard]] virtual auto transfer() noexcept -> std::error_code = 0;

    virtual void clear() noexcept = 0;
};

} // namespace piejam::audio::alsa
