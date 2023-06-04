// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/DbScaleData.h>

#include <piejam/gui/model/DbScaleTick.h>

#include <piejam/functional/in_interval.h>
#include <piejam/math.h>

#include <QObject>
#include <QVector>

#include <algorithm>
#include <limits>

namespace piejam::gui::model
{

float
DbScaleData::dBToPosition(float const dB) const
{
    BOOST_ASSERT(!m_ticks.empty());

    if (dB < m_ticks.front().dB)
    {
        return 0.f;
    }
    else if (dB == m_ticks.back().dB)
    {
        return m_ticks.back().position;
    }
    else if (dB > m_ticks.back().dB)
    {
        return 1.f;
    }
    else
    {
        auto lower = std::ranges::adjacent_find(
                m_ticks,
                [dB](DbScaleTick const& l, DbScaleTick const& r) {
                    return in_right_open(dB, l.dB, r.dB);
                });

        BOOST_ASSERT(lower != m_ticks.end());

        auto upper = std::next(lower);
        constexpr auto inf = std::numeric_limits<float>::infinity();
        if (lower->dB == -inf)
        {
            return lower->position;
        }
        else if (upper->dB == inf)
        {
            return upper->position;
        }
        else
        {
            return math::linear_map(
                    dB,
                    lower->dB,
                    upper->dB,
                    lower->position,
                    upper->position);
        }
    }
}

float
DbScaleData::dBAt(float const position) const
{
    BOOST_ASSERT(!m_ticks.empty());

    constexpr auto inf = std::numeric_limits<float>::infinity();
    if (position < m_ticks.front().position)
    {
        return -inf;
    }
    else if (position == m_ticks.back().position)
    {
        return m_ticks.back().dB;
    }
    else if (position > m_ticks.back().position)
    {
        return inf;
    }
    else
    {
        auto lower = std::ranges::adjacent_find(
                m_ticks,
                [position](DbScaleTick const& l, DbScaleTick const& r) {
                    return in_right_open(position, l.position, r.position);
                });

        BOOST_ASSERT(lower != m_ticks.end());
        auto upper = std::next(lower);
        if (lower->dB == -inf)
        {
            return -inf;
        }
        else if (upper->dB == inf)
        {
            return inf;
        }
        else
        {
            return math::linear_map(
                    position,
                    lower->position,
                    upper->position,
                    lower->dB,
                    upper->dB);
        }
    }
}

} // namespace piejam::gui::model
