// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <optional>
#include <string>

namespace piejam::thread
{

struct configuration
{
    std::optional<int> affinity;
    std::optional<int> realtime_priority;
    std::optional<std::string> name;

    void apply() const;
};

} // namespace piejam::thread
