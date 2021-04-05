// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopePointsGenerator.h>

#include <QPointF>

#include <boost/assert.hpp>

#include <algorithm>
#include <numeric>

namespace piejam::gui::model
{

void
ScopePointsGenerator::update(std::span<float const> const& samples)
{
    BOOST_ASSERT(samples.size() % 2 == 0);
    if (samples.empty())
        return;

    std::vector<QPointF> resultLeft, resultRight;
    std::size_t const resultCapacity = samples.size() / m_samplesPerPoint + 1;
    resultLeft.reserve(resultCapacity);
    resultRight.reserve(resultCapacity);

    for (std::size_t i = 0; i < samples.size(); i += 2)
    {
        float const leftSample = samples[i];
        float const rightSample = samples[i + 1];

        m_accLeftRight.first.min =
                std::min(m_accLeftRight.first.min, leftSample);
        m_accLeftRight.first.max =
                std::max(m_accLeftRight.first.max, leftSample);
        m_accLeftRight.second.min =
                std::min(m_accLeftRight.second.min, rightSample);
        m_accLeftRight.second.max =
                std::max(m_accLeftRight.second.max, rightSample);

        ++m_accNumSamples;

        if (m_accNumSamples >= m_samplesPerPoint)
        {
            resultLeft.emplace_back(
                    m_accLeftRight.first.min,
                    m_accLeftRight.first.max);
            resultLeft.emplace_back(
                    m_accLeftRight.second.min,
                    m_accLeftRight.second.max);

            m_accLeftRight = {};
            m_accNumSamples = 0;
        }
    }

    BOOST_ASSERT(resultLeft.size() == resultRight.size());
    if (!resultLeft.empty())
        emit pointsAdded(resultLeft, resultRight);
}

} // namespace piejam::gui::model
