// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <span>

namespace piejam::system
{

struct cpu_load_data
{
    std::size_t user{};    // time spent in user mode
    std::size_t nice{};    // time spent in user mode with low priority
    std::size_t system{};  // time spent in system mode
    std::size_t idle{};    // time spent in the idle task
    std::size_t iowait{};  // waiting for I/O to complete
    std::size_t irq{};     // servicing hardware interrupts
    std::size_t softirq{}; // servicing software interrupts
    std::size_t steal{};   // other OS when running in a virtualized environment
    std::size_t guest{};   // running a VCPU for guest OS
    std::size_t guest_nice{}; // running a low priority VCPU for guest OS
};

[[nodiscard]]
constexpr auto
active_time(cpu_load_data const& data) -> std::size_t
{
    return data.user + data.nice + data.system + data.irq + data.softirq +
           data.steal + data.guest + data.guest_nice;
}

[[nodiscard]]
constexpr auto
idle_time(cpu_load_data const& data) -> std::size_t
{
    return data.idle + data.iowait;
}

void cpu_load(cpu_load_data& total, std::span<cpu_load_data> per_core) noexcept;

} // namespace piejam::system
