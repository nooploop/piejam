// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::audio
{

enum class pcm_format : unsigned
{
    unsupported,
    s8,
    u8,
    s16_le,
    s16_be,
    u16_le,
    u16_be,
    s32_le,
    s32_be,
    u32_le,
    u32_be,
    s24_3le,
    s24_3be,
    u24_3le,
    u24_3be,
};

} // namespace piejam::audio
