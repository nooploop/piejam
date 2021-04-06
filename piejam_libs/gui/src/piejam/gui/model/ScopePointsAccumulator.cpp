// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopePointsAccumulator.h>

#include <QPointF>

#include <boost/assert.hpp>

#include <algorithm>
#include <numeric>

namespace piejam::gui::model
{

void
ScopePointsAccumulator::update(std::span<float const> const& samples)
{
    BOOST_ASSERT(samples.size() % 2 == 0);
    if (samples.empty())
        return;

    std::vector<ScopePoint> resultLeft, resultRight;
    std::size_t const resultCapacity = samples.size() / m_samplesPerPoint + 1;
    resultLeft.reserve(resultCapacity);
    resultRight.reserve(resultCapacity);

    for (std::size_t i = 0, e = samples.size(); i < e; i += 2)
    {
        float const leftSample = samples[i];
        float const rightSample = samples[i + 1];

        m_accLeft.min = std::min(m_accLeft.min, leftSample);
        m_accLeft.max = std::max(m_accLeft.max, leftSample);
        m_accRight.min = std::min(m_accRight.min, rightSample);
        m_accRight.max = std::max(m_accRight.max, rightSample);

        ++m_accNumSamples;

        if (m_accNumSamples >= m_samplesPerPoint)
        {
            resultLeft.emplace_back(m_accLeft.min, m_accLeft.max);
            resultRight.emplace_back(m_accRight.min, m_accRight.max);

            m_accLeft = {};
            m_accRight = {};
            m_accNumSamples = 0;
        }
    }

    BOOST_ASSERT(resultLeft.size() == resultRight.size());
    if (!resultLeft.empty())
        emit pointsAdded(resultLeft, resultRight);
}

} // namespace piejam::gui::model
