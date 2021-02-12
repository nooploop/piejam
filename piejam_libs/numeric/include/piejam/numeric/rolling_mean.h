// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>

#include <concepts>

namespace piejam::numeric
{

template <std::floating_point T>
class rolling_mean
{
public:
    rolling_mean(std::size_t window_size)
        : m_acc(boost::accumulators::tag::rolling_window::window_size =
                        window_size)
    {
    }

    auto operator()(T x) -> T
    {
        m_acc(x);
        return boost::accumulators::rolling_mean(m_acc);
    }

private:
    boost::accumulators::accumulator_set<
            T,
            boost::accumulators::stats<boost::accumulators::tag::rolling_mean>>
            m_acc;
};

} // namespace piejam::numeric
