// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <optional>

namespace piejam::thread
{

struct configuration
{
    std::optional<int> affinity;
    std::optional<int> priority;
};

} // namespace piejam::audio::engine
