// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

namespace piejam::redux
{

// clang-format off
using flag_resetter =
        std::unique_ptr<bool, decltype([](bool* flag) { *flag = false; })>;
// clang-format on

} // namespace piejam::redux
