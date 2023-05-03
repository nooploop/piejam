// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopeLinesGenerator.h>

#include <piejam/gui/model/ScopeLines.h>

#include <piejam/audio/interleaved_view.h>
#include <piejam/functional/operators.h>
#include <piejam/math.h>
#include <piejam/range/indices.h>

#include <boost/assert.hpp>

#include <stdexcept>

namespace piejam::gui::model
{

namespace
{

struct InactiveGenerator
{
    auto operator()(AudioStreamListener::Stream const&) -> ScopeLines
    {
        return ScopeLines{};
    }
};

[[nodiscard]] constexpr auto
clip(float const v) -> float
{
    return math::clamp(v, -1.f, 1.f);
}

template <std::size_t NumChannels, class FrameConverter>
class Generator
{
public:
    Generator(int const samplesPerPoint)
        : m_samplesPerPoint{samplesPerPoint}
    {
    }

    auto operator()(AudioStreamListener::Stream const& stream) -> ScopeLines
    {
        auto const frames = stream.channels_cast<NumChannels>();

        ScopeLines result;
        FrameConverter convert;

        for (std::span<float const, NumChannels> const frame : frames)
        {
            float const sample = convert(frame);

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

struct ScopeLinesGenerator::Impl
{
    BusType streamType{BusType::Mono};
    int samplesPerLine{1};
    bool active{};
    StereoChannel channel{StereoChannel::Left};

    using Generate =
            std::function<ScopeLines(AudioStreamListener::Stream const&)>;
    Generate generator{InactiveGenerator{}};

    auto makeGenerator() const -> Generate
    {
        if (active)
        {
            switch (streamType)
            {
                case BusType::Mono:
                    return Generator<2, StereoFrameValue<StereoChannel::Left>>{
                            samplesPerLine};

                case BusType::Stereo:
                    switch (channel)
                    {
                        case StereoChannel::Left:
                            return Generator<
                                    2,
                                    StereoFrameValue<StereoChannel::Left>>{
                                    samplesPerLine};

                        case StereoChannel::Right:
                            return Generator<
                                    2,
                                    StereoFrameValue<StereoChannel::Right>>{
                                    samplesPerLine};

                        case StereoChannel::Middle:
                            return Generator<
                                    2,
                                    StereoFrameValue<StereoChannel::Middle>>{
                                    samplesPerLine};

                        case StereoChannel::Side:
                            return Generator<
                                    2,
                                    StereoFrameValue<StereoChannel::Side>>{
                                    samplesPerLine};

                        default:
                            BOOST_ASSERT_MSG(false, "Unknown StereoChannel");
                            return InactiveGenerator{};
                    }
            }
        }
        else
        {
            return InactiveGenerator{};
        }
    }

    void updateGenerator()
    {
        generator = makeGenerator();
    }

    template <class T>
    void updateGeneratorProperty(T& member, T value)
    {
        if (member != value)
        {
            member = std::move(value);
            updateGenerator();
        }
    }
};

ScopeLinesGenerator::ScopeLinesGenerator()
    : m_impl(std::make_unique<Impl>())
{
}

ScopeLinesGenerator::~ScopeLinesGenerator() = default;

void
ScopeLinesGenerator::setStreamType(piejam::gui::model::BusType const streamType)
{
    m_impl->updateGeneratorProperty(m_impl->streamType, streamType);
}

void
ScopeLinesGenerator::setSamplesPerLine(int const samplesPerLine)
{
    if (samplesPerLine <= 0)
    {
        throw std::invalid_argument("samplesPerPoint must be positive");
    }

    m_impl->updateGeneratorProperty(m_impl->samplesPerLine, samplesPerLine);
}

void
ScopeLinesGenerator::setActive(bool const active)
{
    m_impl->updateGeneratorProperty(m_impl->active, active);
}

void
ScopeLinesGenerator::setStereoChannel(StereoChannel const channel)
{
    m_impl->updateGeneratorProperty(m_impl->channel, channel);
}

void
ScopeLinesGenerator::update(Stream const& stream)
{
    generated(m_impl->generator(stream));
}

} // namespace piejam::gui::model
