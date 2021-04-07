// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/fwd.h>

#include <span>

namespace piejam::gui::model
{

class ScopeLinesAccumulator final : public AudioStreamListener
{
    Q_OBJECT

    Q_PROPERTY(int samplesPerLine READ samplesPerLine WRITE setSamplesPerLine
                       NOTIFY samplesPerLineChanged FINAL)

public:
    auto samplesPerLine() const noexcept -> int { return m_samplesPerPoint; }
    void setSamplesPerLine(int x);

    void update(std::span<float const> const&) override;

signals:
    void linesAdded(ScopeLines const& left, ScopeLines const& right);
    void samplesPerLineChanged();

private:
    int m_samplesPerPoint{1};

    float m_accLeftY0{};
    float m_accLeftY1{};
    float m_accRightY0{};
    float m_accRightY1{};
    int m_accNumSamples{};
};

} // namespace piejam::gui::model
