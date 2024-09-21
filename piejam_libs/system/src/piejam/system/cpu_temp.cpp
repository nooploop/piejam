// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/system/cpu_temp.h>

#include <fstream>

namespace piejam::system
{

auto
cpu_temp() noexcept -> int
{
    try
    {
        std::ifstream cpu_temp_file("/sys/class/thermal/thermal_zone0/temp");
        int cpu_temp{};
        cpu_temp_file >> cpu_temp;
        cpu_temp /= 1000;
        return cpu_temp;
    }
    catch (std::exception const&)
    {
        return 0;
    }
}

} // namespace piejam::system
