// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <filesystem>

namespace piejam::runtime
{

struct locations
{
    std::filesystem::path config_dir;
    std::filesystem::path home_dir;
    std::filesystem::path rec_dir;
};

} // namespace piejam::runtime
