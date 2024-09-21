// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/DbScaleData.h>

#include <piejam/gui/model/DbScaleTick.h>

#include <piejam/functional/in_interval.h>
#include <piejam/math.h>

#include <QObject>
#include <QVector>

#include <algorithm>
#include <cmath>
#include <limits>

namespace piejam::gui::model
{

static constexpr float s_min_dB = -160.f;
static constexpr float s_inf = std::numeric_limits<float>::infinity();

float
DbScaleData::dBToPosition(float const dB) const
{
    BOOST_ASSERT(!std::isnan(dB));
    BOOST_ASSERT(!m_ticks.empty());

    if (dB < s_min_dB)
    {
        return 0.f;
    }

    if (dB == m_ticks.back().dB)
    {
        return m_ticks.back().position;
    }

    if (dB > m_ticks.back().dB)
    {
        return 1.f;
    }

    auto const lower = std::ranges::adjacent_find(
            m_ticks,
            [dB](DbScaleTick const& l, DbScaleTick const& r) {
                return in_right_open(dB, l.dB, r.dB);
            });

    BOOST_ASSERT(lower != m_ticks.end());

    auto const upper = std::next(lower);

    if (lower->dB == -s_inf)
    {
        return math::linear_map(
                dB,
                s_min_dB,
                upper->dB,
                lower->position,
                upper->position);
    }

    if (upper->dB == s_inf)
    {
        return upper->position;
    }

    return math::linear_map(
            dB,
            lower->dB,
            upper->dB,
            lower->position,
            upper->position);
}

float
DbScaleData::dBAt(float const position) const
{
    BOOST_ASSERT(!m_ticks.empty());

    if (position == 0. || position < m_ticks.front().position)
    {
        return -s_inf;
    }

    if (position == m_ticks.back().position)
    {
        return m_ticks.back().dB;
    }

    if (position > m_ticks.back().position)
    {
        return s_inf;
    }

    auto const lower = std::ranges::adjacent_find(
            m_ticks,
            [position](DbScaleTick const& l, DbScaleTick const& r) {
                return in_right_open(position, l.position, r.position);
            });

    BOOST_ASSERT(lower != m_ticks.end());
    auto const upper = std::next(lower);
    if (lower->dB == -s_inf)
    {
        return math::linear_map(
                position,
                lower->position,
                upper->position,
                s_min_dB,
                upper->dB);
    }

    if (upper->dB == s_inf)
    {
        return s_inf;
    }

    return math::linear_map(
            position,
            lower->position,
            upper->position,
            lower->dB,
            upper->dB);
}

} // namespace piejam::gui::model
