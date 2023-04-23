// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/system/cpu_load.h>

#include <span>
#include <vector>

namespace piejam::system
{

class avg_cpu_load_tracker final
{
public:
    explicit avg_cpu_load_tracker(std::size_t num_cores);

    [[nodiscard]] auto total() const noexcept -> float
    {
        return m_total_avg;
    }

    [[nodiscard]] auto per_core() const noexcept -> std::span<float const>
    {
        return m_per_core_avg;
    }

    void update();

private:
    cpu_load_data m_total_data;
    std::vector<cpu_load_data> m_per_core_data;

    float m_total_avg{};
    std::vector<float> m_per_core_avg;
};

} // namespace piejam::system
