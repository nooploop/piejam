// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/ScopePoint.h>

#include <boost/assert.hpp>

#include <span>

namespace piejam::gui::model
{

class ScopePointsAccumulator final : public AudioStreamListener
{
    Q_OBJECT

    Q_PROPERTY(int samplesPerPoint READ samplesPerPoint WRITE setSamplesPerPoint
                       NOTIFY samplesPerPointChanged FINAL)

public:
    auto samplesPerPoint() const noexcept -> int { return m_samplesPerPoint; }
    void setSamplesPerPoint(int x)
    {
        if (x <= 0)
            throw std::invalid_argument("samplesPerPoint must be positive");

        if (m_samplesPerPoint != x)
        {
            m_samplesPerPoint = x;
            emit samplesPerPointChanged();
        }
    }

    void update(std::span<float const> const&) override;

signals:
    void pointsAdded(
            std::vector<ScopePoint> const& left,
            std::vector<ScopePoint> const& right);
    void samplesPerPointChanged();

private:
    int m_samplesPerPoint{1};

    ScopePoint m_accLeft;
    ScopePoint m_accRight;
    int m_accNumSamples{};
};

} // namespace piejam::gui::model
