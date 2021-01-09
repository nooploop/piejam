// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>

namespace piejam::runtime
{

using channel_index_pair = audio::pair<std::size_t>;

} // namespace piejam::runtime
