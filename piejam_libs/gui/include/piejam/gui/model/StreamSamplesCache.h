// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/shift_push_back.h>

#include <boost/range/adaptor/strided.hpp>

#include <ranges>
#include <span>
#include <vector>

namespace piejam::gui::model
{

class StreamSamplesCache
{
public:
    StreamSamplesCache(std::size_t windowSize = 1024, int stride = 1)
        : m_cached(windowSize)
        , m_stride{stride}
    {
    }

    [[nodiscard]]
    auto cached() const noexcept -> std::span<float const>
    {
        return m_cached;
    }

    template <class Samples>
    void process(Samples samples)
    {
        auto const streamFramesSubRange = boost::make_iterator_range(
                std::next(
                        std::begin(samples),
                        m_restFrames == 0 ? 0 : m_stride - m_restFrames),
                std::end(samples));

        algorithm::shift_push_back(
                m_cached,
                boost::adaptors::stride(streamFramesSubRange, m_stride));

        m_restFrames =
                static_cast<int>(std::ranges::size(streamFramesSubRange)) %
                m_stride;
    }

private:
    std::vector<float> m_cached;
    int m_stride{1};
    int m_restFrames{};
};

} // namespace piejam::gui::model
