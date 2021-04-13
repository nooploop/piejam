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

    void update(range::interleaved_view<float const, 0> const&) override;

signals:
    void samplesPerLineChanged();

    void linesAdded(std::size_t channel, ScopeLines const&);

private:
    int m_samplesPerPoint{1};

    struct Acc
    {
        float y0;
        float y1;
    };

    std::vector<Acc> m_acc;
    int m_accNumSamples{};
};

} // namespace piejam::gui::model
