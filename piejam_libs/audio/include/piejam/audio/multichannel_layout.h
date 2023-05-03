// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::audio
{

enum class multichannel_layout : bool
{
    non_interleaved,
    interleaved,
};

} // namespace piejam::audio
