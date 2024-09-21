// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/ui/fwd.h>

#include <piejam/runtime/ui/action.h>

#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/core/demangle.hpp>

#include <chrono>
#include <fstream>
#include <typeindex>
#include <unordered_map>

namespace piejam::runtime::ui
{

class action_tracker_middleware
{
public:
    action_tracker_middleware(next_f next, std::string name)
        : m_next(std::move(next))
        , m_name(std::move(name))
    {
    }

    ~action_tracker_middleware()
    {
        std::ofstream stats_file(m_name);

        for (auto const& [ty, acc] : m_stats)
        {
            stats_file << boost::core::demangle(ty.name()) << ", "
                       << boost::accumulators::count(acc) << ", "
                       << std::chrono::duration_cast<std::chrono::microseconds>(
                                  boost::accumulators::mean(acc))
                                  .count()
                       << std::endl;
        }
    }

    void operator()(action const& a)
    {
        auto const started = std::chrono::steady_clock::now();
        m_next(a);
        auto const stopped = std::chrono::steady_clock::now();

        m_stats[typeid(a)](stopped - started);
    }

private:
    next_f m_next;
    std::string m_name;

    using acc_t = boost::accumulators::accumulator_set<
            std::chrono::steady_clock::duration,
            boost::accumulators::stats<
                    boost::accumulators::tag::count,
                    boost::accumulators::tag::mean>>;

    std::unordered_map<std::type_index, acc_t> m_stats;
};

} // namespace piejam::runtime::ui
