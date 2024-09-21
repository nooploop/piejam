// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string_view>

namespace piejam::this_thread
{

void set_name(std::string_view);

} // namespace piejam::this_thread
