// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/system/avg_cpu_load_tracker.h>

#include <algorithm>

namespace piejam::system
{

avg_cpu_load_tracker::avg_cpu_load_tracker(std::size_t num_cores)
    : m_per_core_data(num_cores)
    , m_per_core_avg(num_cores)
{
    cpu_load(m_total_data, m_per_core_data);
}

static auto
calc_avg_load(cpu_load_data const& prev, cpu_load_data const& next) -> float
{
    float const active = active_time(next) - active_time(prev);
    float const idle = idle_time(next) - idle_time(prev);
    float const total = active + idle;
    return total ? active / total : 0.f;
}

void
avg_cpu_load_tracker::update()
{
    auto const prev_total = m_total_data;
    auto const prev_per_core = m_per_core_data;

    cpu_load(m_total_data, m_per_core_data);

    m_total_avg = calc_avg_load(prev_total, m_total_data);

    std::ranges::transform(
            prev_per_core,
            m_per_core_data,
            m_per_core_avg.begin(),
            &calc_avg_load);
}

} // namespace piejam::system
