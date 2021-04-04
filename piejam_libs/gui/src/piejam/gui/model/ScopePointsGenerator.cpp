// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopePointsGenerator.h>

#include <QPointF>

#include <algorithm>
#include <numeric>

namespace piejam::gui::model
{

void
ScopePointsGenerator::update(std::span<float const> const& samples)
{
    std::vector<QPointF> result;

    m_unprocessed.insert(m_unprocessed.end(), samples.begin(), samples.end());

    std::size_t samplesProcessed = 0;
    for (; (samplesProcessed + m_samplesPerPoint) < m_unprocessed.size();
         samplesProcessed += m_samplesPerPoint)
    {
        result.emplace_back(std::accumulate(
                std::next(m_unprocessed.begin(), samplesProcessed),
                std::next(
                        m_unprocessed.end(),
                        samplesProcessed + m_samplesPerPoint),
                QPointF(),
                [](QPointF const& p, float const sample) {
                    return QPointF(
                            std::max(p.x(), static_cast<qreal>(sample)),
                            std::min(p.y(), static_cast<qreal>(sample)));
                }));
    }

    if (samplesProcessed)
        m_unprocessed.erase(
                m_unprocessed.begin(),
                std::next(m_unprocessed.begin(), samplesProcessed));

    emit pointsAdded(result);
}

} // namespace piejam::gui::model
