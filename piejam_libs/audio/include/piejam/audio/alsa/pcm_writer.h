// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/fwd.h>

#include <system_error>

namespace piejam::audio::alsa
{

class pcm_writer
{
public:
    virtual ~pcm_writer() = default;

    virtual auto buffer() noexcept -> range::table_view<float> = 0;

    [[nodiscard]] virtual auto write() noexcept -> std::error_code = 0;
};

} // namespace piejam::audio::alsa
