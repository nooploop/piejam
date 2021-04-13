// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopeLinesAccumulator.h>

#include <piejam/gui/model/ScopeLines.h>
#include <piejam/math.h>
#include <piejam/range/indices.h>
#include <piejam/range/interleaved_view.h>

#include <boost/assert.hpp>

#include <stdexcept>

namespace piejam::gui::model
{

void
ScopeLinesAccumulator::setSamplesPerLine(int x)
{
    if (x <= 0)
        throw std::invalid_argument("samplesPerPoint must be positive");

    if (m_samplesPerPoint != x)
    {
        m_samplesPerPoint = x;
        emit samplesPerLineChanged();
    }
}

static constexpr auto
clip(float const v) -> float
{
    return math::clamp(v, -1.f, 1.f);
}

void
ScopeLinesAccumulator::update(
        range::interleaved_view<float const> const& stream)
{
    if (stream.empty())
        return;

    if (m_acc.size() != stream.num_channels())
    {
        m_acc.resize(stream.num_channels());
        m_accNumSamples = 0;
    }

    if (m_acc.size() == 0)
        return;

    std::vector<ScopeLines> result(m_acc.size());
    std::ranges::for_each(
            result,
            [resultCapacity = stream.size() / m_samplesPerPoint + 1](
                    auto& scopeLines) { scopeLines.reserve(resultCapacity); });

    for (auto const frame : stream)
    {
        if (m_accNumSamples) [[likely]]
        {
            for (std::size_t i = 0; i < frame.size(); ++i)
            {
                auto& acc = m_acc[i];
                float const sample = frame[i];
                acc.y0 = std::min(acc.y0, sample);
                acc.y1 = std::max(acc.y1, sample);
            }
        }
        else
        {
            for (std::size_t i = 0; i < frame.size(); ++i)
            {
                float const sample = frame[i];
                m_acc[i] = Acc{.y0 = sample, .y1 = sample};
            }
        }

        ++m_accNumSamples;

        if (m_accNumSamples >= m_samplesPerPoint)
        {
            for (std::size_t i = 0; i < frame.size(); ++i)
            {
                auto const& acc = m_acc[i];
                result[i].push_back(clip(acc.y0), clip(acc.y1));
            }

            m_accNumSamples = 0;
        }
    }

    if (result[0].empty())
        return;

    for (std::size_t i : range::indices(result))
        emit linesAdded(i, result[i]);
}

} // namespace piejam::gui::model
