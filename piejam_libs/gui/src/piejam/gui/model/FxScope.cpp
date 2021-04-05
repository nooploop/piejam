// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxScope.h>

#include <piejam/gui/model/ScopePointsGenerator.h>

namespace piejam::gui::model
{

FxScope::FxScope(QObject* parent)
    : QObject(parent)
    , m_streamListener(std::make_unique<ScopePointsGenerator>())
{
}

FxScope::~FxScope() = default;

auto
FxScope::streamListener() const noexcept -> AudioStreamListener*
{
    return m_streamListener.get();
}

void
FxScope::setSamplesPerPoint(int const x)
{
    if (m_samplesPerPoint != x)
    {
        m_streamListener->setSamplesPerPoint(x);
        m_samplesPerPoint = x;

        emit samplesPerPointChanged();
    }
}

void
FxScope::setViewSize(int const x)
{
    if (m_viewSize != x)
    {
        m_viewSize = x;
        emit viewSizeChanged();

        clear();
    }
}

void
FxScope::clear()
{
    m_leftPoints = std::vector<QPointF>(m_viewSize);
    emit leftPointsChanged();
    m_rightPoints = std::vector<QPointF>(m_viewSize);
    emit rightPointsChanged();
}

} // namespace piejam::gui::model
