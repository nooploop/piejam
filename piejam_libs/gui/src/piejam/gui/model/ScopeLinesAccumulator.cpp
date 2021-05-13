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

    static constexpr auto frameValue(std::span<float const, 2> const& frame)
            -> float
    {
        switch (SC)
        {
            case StereoChannel::Left:
                return frame[0];
            case StereoChannel::Right:
                return frame[1];
            case StereoChannel::Middle:
                return frame[0] + frame[1];
            case StereoChannel::Side:
                return frame[0] - frame[1];
        }
    }

    auto operator()(AudioStreamListener::Stream const& stream)
            -> std::optional<ScopeLines>
    {
        auto const stereoView = stream.channels_cast<2>();

        ScopeLines result;

        for (auto const frame : stereoView)
        {
            float const sample = frameValue(frame);

            if (m_accNumSamples) [[likely]]
            {
                m_acc.y0 = std::min(m_acc.y0, sample);
                m_acc.y1 = std::max(m_acc.y1, sample);
            }
            else
            {
                m_acc.y0 = sample;
                m_acc.y1 = sample;
            }

            ++m_accNumSamples;

            if (m_accNumSamples >= m_samplesPerPoint)
            {
                result.push_back(clip(m_acc.y0), clip(m_acc.y1));

                m_accNumSamples = 0;
            }
        }

        return result;
    }

private:
    int m_samplesPerPoint{1};

    struct Acc
    {
        float y0;
        float y1;
    };

    Acc m_acc;
    int m_accNumSamples{};
};

} // namespace

struct ScopeLinesAccumulator::Impl
{
    int samplesPerPoint{1};
    bool active{};
    StereoChannel channel{StereoChannel::Left};
    std::function<std::optional<ScopeLines>(AudioStreamListener::Stream const&)>
            generator{InactiveGenerator{}};

    void updateGenerator()
    {
        if (active)
        {
            switch (channel)
            {
                case StereoChannel::Left:
                    generator = Generator<StereoChannel::Left>{samplesPerPoint};
                    break;

                case StereoChannel::Right:
                    generator =
                            Generator<StereoChannel::Right>{samplesPerPoint};
                    break;

                case StereoChannel::Middle:
                    generator =
                            Generator<StereoChannel::Middle>{samplesPerPoint};
                    break;

                case StereoChannel::Side:
                    generator = Generator<StereoChannel::Side>{samplesPerPoint};
                    break;

                default:
                    BOOST_ASSERT_MSG(false, "Unknown StereoChannel");
                    generator = InactiveGenerator{};
                    break;
            }
        }
        else
        {
            generator = InactiveGenerator{};
        }
    }
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
