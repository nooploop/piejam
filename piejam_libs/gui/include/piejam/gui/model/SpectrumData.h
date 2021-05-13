// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SpectrumDataPoint.h>

#include <QObject>

#include <vector>

namespace piejam::gui::model
{

class SpectrumData : public QObject
{
    Q_OBJECT
public:
    auto get() const noexcept -> SpectrumDataPoints { return m_dataPoints; }

    void set(SpectrumDataPoints const& dataPoints)
    {
        m_dataPoints = std::vector<SpectrumDataPoint>(
                dataPoints.begin(),
                dataPoints.end());
        emit changed();
    }

    void clear()
    {
        m_dataPoints.clear();
        emit changed();
    }

signals:
    void changed();

private:
    std::vector<SpectrumDataPoint> m_dataPoints;
};

} // namespace piejam::gui::model
