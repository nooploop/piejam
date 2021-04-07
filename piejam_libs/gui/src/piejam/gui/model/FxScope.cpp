// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxScope.h>

#include <piejam/gui/model/ScopeLines.h>
#include <piejam/gui/model/ScopeLinesAccumulator.h>

namespace piejam::gui::model
{

struct FxScope::Impl
{
    ScopeLinesAccumulator streamListener;
    int samplesPerPoint{1};
    int viewSize{};
    ScopeLinesObject leftLines;
    ScopeLinesObject rightLines;
};

FxScope::FxScope(QObject* parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
    connect(&m_impl->streamListener,
            &ScopeLinesAccumulator::linesAdded,
            [this](ScopeLines const& addedLeft, ScopeLines const& addedRight) {
                m_impl->leftLines.get().shift_push_back(addedLeft);
                m_impl->leftLines.update();

                m_impl->rightLines.get().shift_push_back(addedRight);
                m_impl->rightLines.update();
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
        m_impl->streamListener.setSamplesPerLine(x);
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
FxScope::leftLines() noexcept -> ScopeLinesObject*
{
    return &m_impl->leftLines;
}

auto
FxScope::rightLines() noexcept -> ScopeLinesObject*
{
    return &m_impl->rightLines;
}

void
FxScope::clear()
{
    m_impl->leftLines.get().clear();
    m_impl->leftLines.get().resize(m_impl->viewSize);
    m_impl->leftLines.update();

    m_impl->rightLines.get().clear();
    m_impl->rightLines.get().resize(m_impl->viewSize);
    m_impl->rightLines.update();
}

} // namespace piejam::gui::model
