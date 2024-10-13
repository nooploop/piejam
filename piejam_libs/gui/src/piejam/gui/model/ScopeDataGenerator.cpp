// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopeDataGenerator.h>

#include <piejam/functional/edge_detect.h>

#include <boost/assert.hpp>
#include <boost/container/static_vector.hpp>

#include <ranges>

namespace piejam::gui::model
{

namespace
{

auto
findTrigger(
        std::span<float const> samples,
        std::size_t const windowSize,
        TriggerSlope const trigger,
        float triggerLevel)
{
    if (samples.empty())
    {
        return samples.end();
    }

    BOOST_ASSERT(samples.size() >= windowSize);

    auto itFirst = samples.begin();
    auto itLast = std::next(
            samples.begin(),
            static_cast<std::ranges::range_difference_t<ScopeData::Samples>>(
                    samples.size() - windowSize));

    auto itFound =
            trigger == TriggerSlope::RisingEdge
                    ? std::adjacent_find(
                              itFirst,
                              itLast,
                              std::bind_front(rising_edge, triggerLevel))
                    : std::adjacent_find(
                              itFirst,
                              itLast,
                              std::bind_front(falling_edge, triggerLevel));

    if (itFound == itLast)
    {
        return samples.end();
    }

    return itFound;
}

} // namespace

auto
ScopeDataGenerator::process(
        std::size_t triggerStream,
        Streams streams,
        std::size_t windowSize,
        TriggerSlope triggerSlope,
        float triggerLevel,
        std::size_t capturedFrames,
        std::size_t holdTimeInFrames) -> Streams
{
    BOOST_ASSERT(
            streams.empty() || streams.size() == 1 ||
            streams[0].size() == streams[1].size());

    auto const triggerStreamSamples = streams[triggerStream];

    Streams result;

    switch (m_state)
    {
        case ScopeDataGeneratorState::WaitingForTrigger:
        {
            auto it = findTrigger(
                    triggerStreamSamples,
                    windowSize,
                    triggerSlope,
                    triggerLevel);
            if (it != triggerStreamSamples.end())
            {
                auto offset = std::distance(triggerStreamSamples.begin(), it);
                std::ranges::transform(
                        streams,
                        std::back_inserter(result),
                        [=](std::span<float const> stream) {
                            auto r = std::span{
                                    stream.data() + offset,
                                    windowSize};
                            BOOST_ASSERT(r.end() <= stream.end());
                            return r;
                        });

                m_state = ScopeDataGeneratorState::Hold;
                m_holdCapturedSize = 0;
            }
            break;
        }

        case ScopeDataGeneratorState::Hold:
        {
            m_holdCapturedSize += capturedFrames;
            if (m_holdCapturedSize >= holdTimeInFrames)
            {
                m_state = ScopeDataGeneratorState::WaitingForTrigger;
            }
            break;
        }
    }

    return result;
}

} // namespace piejam::gui::model
