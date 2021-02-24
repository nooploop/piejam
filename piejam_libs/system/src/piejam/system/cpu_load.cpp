// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/system/cpu_load.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <array>
#include <fstream>
#include <sstream>

namespace piejam::system
{

namespace
{

auto
parse_cpu_line(std::string line) -> std::pair<std::string, cpu_load_data>
{
    std::pair<std::string, cpu_load_data> result;

    std::istringstream iss(line);

    iss >> result.first >> result.second.user >> result.second.nice >>
            result.second.system >> result.second.idle >>
            result.second.iowait >> result.second.irq >>
            result.second.softirq >> result.second.steal >>
            result.second.guest >> result.second.guest_nice;

    return result;
}

} // namespace

void
cpu_load(cpu_load_data& total, std::span<cpu_load_data> per_core) noexcept
{
    static std::string s_cpu{"cpu"};

    try
    {
        std::ifstream stat_file("/proc/stat");

        std::array<char, 1024> line;
        while (stat_file.getline(line.data(), line.size()))
        {
            if (boost::starts_with(line, s_cpu))
            {
                auto parsed_data = parse_cpu_line(line.data());
                if (parsed_data.first == s_cpu)
                {
                    total = parsed_data.second;
                }
                else
                {
                    boost::algorithm::erase_head(parsed_data.first, 3);
                    auto core_num = std::stoul(parsed_data.first);
                    if (core_num < per_core.size())
                    {
                        per_core[core_num] = parsed_data.second;
                    }
                }
            }
        }
    }
    catch (std::exception const&)
    {
    }
}

} // namespace piejam::system
