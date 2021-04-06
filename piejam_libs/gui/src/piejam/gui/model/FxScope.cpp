// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxScope.h>

#include <piejam/algorithm/shift_push_back.h>
#include <piejam/gui/model/ScopePointsAccumulator.h>
#include <piejam/range/indices.h>

#include <algorithm>

namespace piejam::gui::model
{

namespace
{

auto
toScopeLines(std::vector<ScopePoint> const& scopePoints) -> QVector<QLineF>
{
    QVector<QLineF> result;
    result.reserve(scopePoints.size() * 2);

    for (std::size_t i : range::indices(scopePoints))
    {
        auto const& sp = scopePoints[i];
        result.push_back(QLineF(i, sp.min, i, sp.max));
    }

    return result;
}

} // namespace

struct FxScope::Impl
{
    ScopePointsAccumulator streamListener;
    int samplesPerPoint{1};
    int viewSize{};
    std::vector<ScopePoint> leftScopePoints;
    ScopeLines leftLines;
    std::vector<ScopePoint> rightScopePoints;
    ScopeLines rightLines;
};

FxScope::FxScope(QObject* parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
    connect(&m_impl->leftLines,
            &ScopeLines::linesChanged,
            this,
            &FxScope::leftLinesChanged);

    connect(&m_impl->rightLines,
            &ScopeLines::linesChanged,
            this,
            &FxScope::rightLinesChanged);

    connect(&m_impl->streamListener,
            &ScopePointsAccumulator::pointsAdded,
            [this](auto const& addedLeft, auto const& addedRight) {
                algorithm::shift_push_back(m_impl->leftScopePoints, addedLeft);
                m_impl->leftLines.setLines(
                        toScopeLines(m_impl->leftScopePoints));

                algorithm::shift_push_back(
                        m_impl->rightScopePoints,
                        addedRight);
                m_impl->rightLines.setLines(
                        toScopeLines(m_impl->rightScopePoints));
            });
}

FxScope::~FxScope() = default;

auto
FxScope::streamListener() noexcept -> AudioStreamListener*
{
    return &m_impl->streamListener;
}

auto
FxScope::samplesPerPoint() const noexcept -> int
{
    return m_impl->samplesPerPoint;
}

void
FxScope::setSamplesPerPoint(int const x)
{
    if (m_impl->samplesPerPoint != x)
    {
        m_impl->streamListener.setSamplesPerPoint(x);
        m_impl->samplesPerPoint = x;

        emit samplesPerPointChanged();
    }
}

auto
FxScope::viewSize() const noexcept -> int
{
    return m_impl->viewSize;
}

void
FxScope::setViewSize(int const x)
{
    if (m_impl->viewSize != x)
    {
        m_impl->viewSize = x;
        emit viewSizeChanged();

        clear();
    }
}

auto
FxScope::leftLines() noexcept -> ScopeLines*
{
    return &m_impl->leftLines;
}

auto
FxScope::rightLines() noexcept -> ScopeLines*
{
    return &m_impl->rightLines;
}

void
FxScope::clear()
{
    m_impl->leftScopePoints = std::vector<ScopePoint>(m_impl->viewSize);
    m_impl->leftLines.setLines(toScopeLines(m_impl->leftScopePoints));

    m_impl->rightScopePoints = std::vector<ScopePoint>(m_impl->viewSize);
    m_impl->rightLines.setLines(toScopeLines(m_impl->rightScopePoints));
}

} // namespace piejam::gui::model
