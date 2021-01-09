// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/fwd.h>

#include <system_error>

namespace piejam::audio::alsa
{

class pcm_reader
{
public:
    virtual ~pcm_reader() = default;

    virtual auto buffer() const noexcept -> range::table_view<float const> = 0;

    [[nodiscard]] virtual auto read() noexcept -> std::error_code = 0;

    virtual void clear_buffer() noexcept = 0;
};

} // namespace piejam::audio::alsa
