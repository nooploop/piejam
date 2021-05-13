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

namespace
{

struct InactiveGenerator
{
    auto operator()(AudioStreamListener::Stream const&)
            -> std::optional<ScopeLines>
    {
        return ScopeLines();
    }
};

constexpr auto
clip(float const v) -> float
{
    return math::clamp(v, -1.f, 1.f);
}

template <StereoChannel SC>
class Generator
{
public:
    Generator(int const samplesPerPoint)
        : m_samplesPerPoint(samplesPerPoint)
    {
    }

    auto operator()(AudioStreamListener::Stream const& stream)
            -> std::optional<ScopeLines>
    {
        auto const stereoView = stream.channels_cast<2>();

        ScopeLines result;

        for (auto const frame : stereoView)
        {
            float const sample = frameValue<SC>(frame);

            if (m_accNumSamples) [[likely]]
            {
                m_accY0 = std::min(m_accY0, sample);
                m_accY1 = std::max(m_accY1, sample);
            }
            else
            {
                m_accY0 = sample;
                m_accY1 = sample;
            }

            ++m_accNumSamples;

            if (m_accNumSamples >= m_samplesPerPoint)
            {
                result.push_back(clip(m_accY0), clip(m_accY1));

                m_accNumSamples = 0;
            }
        }

        return result;
    }

private:
    int m_samplesPerPoint{1};

    float m_accY0{};
    float m_accY1{};
    int m_accNumSamples{};
};

} // namespace

struct ScopeLinesAccumulator::Impl
{
    int samplesPerPoint{1};
    bool active{};
    StereoChannel channel{StereoChannel::Left};

    using Generate = std::function<std::optional<ScopeLines>(
            AudioStreamListener::Stream const&)>;
    Generate generator{InactiveGenerator{}};

    auto makeGenerator() const -> Generate
    {
        if (active)
        {
            switch (channel)
            {
                case StereoChannel::Left:
                    return Generator<StereoChannel::Left>{samplesPerPoint};

                case StereoChannel::Right:
                    return Generator<StereoChannel::Right>{samplesPerPoint};

                case StereoChannel::Middle:
                    return Generator<StereoChannel::Middle>{samplesPerPoint};

                case StereoChannel::Side:
                    return Generator<StereoChannel::Side>{samplesPerPoint};

                default:
                    BOOST_ASSERT_MSG(false, "Unknown StereoChannel");
                    return InactiveGenerator{};
            }
        }
        else
        {
            return InactiveGenerator{};
        }
    }

    void updateGenerator() { generator = makeGenerator(); }
};

ScopeLinesAccumulator::ScopeLinesAccumulator()
    : m_impl(std::make_unique<Impl>())
{
}

ScopeLinesAccumulator::~ScopeLinesAccumulator() = default;

void
ScopeLinesAccumulator::setSamplesPerLine(int const samplesPerLine)
{
    if (samplesPerLine <= 0)
        throw std::invalid_argument("samplesPerPoint must be positive");

    if (m_impl->samplesPerPoint != samplesPerLine)
    {
        m_impl->samplesPerPoint = samplesPerLine;
        m_impl->updateGenerator();
    }
}

void
ScopeLinesAccumulator::setActive(bool const active)
{
    if (m_impl->active != active)
    {
        m_impl->active = active;
        m_impl->updateGenerator();
    }
}

void
ScopeLinesAccumulator::setChannel(StereoChannel const channel)
{
    if (m_impl->channel != channel)
    {
        m_impl->channel = channel;
        m_impl->updateGenerator();
    }
}

void
ScopeLinesAccumulator::update(Stream const& stream)
{
    if (auto lines = m_impl->generator(stream))
        generated(*lines);
}

} // namespace piejam::gui::model
