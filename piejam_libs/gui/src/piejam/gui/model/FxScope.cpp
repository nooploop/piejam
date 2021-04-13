// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxScope.h>

#include <piejam/gui/model/ScopeLines.h>
#include <piejam/gui/model/ScopeLinesAccumulator.h>

#include <array>

namespace piejam::gui::model
{

struct FxScope::Impl
{
    ScopeLinesAccumulator streamListener;
    int samplesPerPoint{1};
    int viewSize{};
    std::array<ScopeLinesObject, 2> lines;
};

FxScope::FxScope(QObject* parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
    connect(&m_impl->streamListener,
            &ScopeLinesAccumulator::linesAdded,
            [this](std::size_t const channel, ScopeLines const& addedLines) {
                BOOST_ASSERT(channel < 2);
                m_impl->lines[channel].get().shift_push_back(addedLines);
                m_impl->lines[channel].update();
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
    return &m_impl->lines[0];
}

auto
FxScope::rightLines() noexcept -> ScopeLinesObject*
{
    return &m_impl->lines[1];
}

void
FxScope::clear()
{
    for (auto& obj : m_impl->lines)
    {
        obj.get().clear();
        obj.get().resize(m_impl->viewSize);
        obj.update();
    }
}

} // namespace piejam::gui::model
