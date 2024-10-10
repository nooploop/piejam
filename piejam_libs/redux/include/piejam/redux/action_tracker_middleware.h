// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/middleware_functors.h>

#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/core/demangle.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <typeindex>
#include <unordered_map>

namespace piejam::redux
{

class action_tracker_middleware
{
public:
    action_tracker_middleware(std::filesystem::path stats_path)
        : m_stats_path(std::move(stats_path))
    {
    }

    ~action_tracker_middleware()
    {
        std::ofstream stats_file(m_stats_path);

        for (auto const& [ty, acc] : m_stats)
        {
            stats_file << boost::core::demangle(ty.name()) << ", "
                       << boost::accumulators::count(acc) << ", "
                       << std::chrono::duration_cast<std::chrono::microseconds>(
                                  boost::accumulators::mean(acc))
                                  .count()
                       << " us\n";
        }
    }

    template <class State, class Action>
    void operator()(
            redux::middleware_functors<State, Action> const& mw_fs,
            Action const& a)
    {
        auto const started = std::chrono::steady_clock::now();
        mw_fs.next(a);
        auto const stopped = std::chrono::steady_clock::now();

        m_stats[typeid(a)](stopped - started);
    }

private:
    std::filesystem::path m_stats_path;

    using acc_t = boost::accumulators::accumulator_set<
            std::chrono::steady_clock::duration,
            boost::accumulators::stats<
                    boost::accumulators::tag::count,
                    boost::accumulators::tag::mean>>;

    std::unordered_map<std::type_index, acc_t> m_stats;
};

} // namespace piejam::redux
