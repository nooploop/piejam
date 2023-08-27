// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ScopeDataGenerator.h>

#include <piejam/gui/model/StreamProcessor.h>

#include <piejam/audio/sample_rate.h>
#include <piejam/functional/edge_detect.h>
#include <piejam/math.h>

#include <boost/assert.hpp>
#include <boost/hof/construct.hpp>
#include <boost/range/adaptor/strided.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

#include <vector>

namespace piejam::gui::model
{

namespace
{

using ScopeSamples = std::span<float const>;

struct Args
{
    std::size_t resolution{1};
};

struct InactiveGenerator final : SubStreamProcessor<ScopeData::Samples>
{
    InactiveGenerator(Args const&)
    {
    }

    auto process(AudioStream const& stream) -> ScopeData::Samples
    {
        m_captured.resize(m_captured.size() + stream.num_frames());
        return m_captured;
    }

    void drop(std::size_t const frames)
    {
        BOOST_ASSERT(frames < m_captured.size());
        m_captured.erase(
                m_captured.begin(),
                std::next(m_captured.begin() + frames));
    }

    void clear()
    {
        m_captured.clear();
    }

private:
    std::vector<float> m_captured{};
};

template <StereoChannel SC>
class Generator final : public SubStreamProcessor<ScopeData::Samples>
{
public:
    Generator(Args args)
        : m_args{std::move(args)}
    {
    }

    auto process(AudioStream const& stream) -> ScopeData::Samples override
    {
        BOOST_ASSERT(
                stream.layout() == audio::multichannel_layout::non_interleaved);

        auto streamFrames = [&]() {
            if constexpr (SC != StereoChannel::Left)
            {
                return stream.channels_cast<2>().frames();
            }
            else
            {
                return stream.frames();
            }
        }();

        auto const streamFramesSubRange = boost::make_iterator_range(
                std::next(
                        streamFrames.begin(),
                        m_restFrames == 0 ? 0
                                          : m_args.resolution - m_restFrames),
                streamFrames.end());

        boost::push_back(
                m_captured,
                streamFramesSubRange |
                        boost::adaptors::strided(m_args.resolution) |
                        boost::adaptors::transformed([](auto frame) {
                            return math::clamp(
                                    StereoFrameValue<SC>{}(frame),
                                    -1.f,
                                    1.f);
                        }));

        m_restFrames = streamFramesSubRange.size() % m_args.resolution;

        return m_captured;
    }

    void drop(std::size_t const frames) override
    {
        BOOST_ASSERT(frames < m_captured.size());
        m_captured.erase(
                m_captured.begin(),
                std::next(m_captured.begin() + frames));
    }

    void clear() override
    {
        m_captured.clear();
    }

private:
    Args m_args;
    std::size_t m_restFrames{0};
    std::vector<float> m_captured{};
};

struct Factory
{
    auto operator()(SubStreamProcessorConfig const& config, Args const& args)
            -> std::unique_ptr<SubStreamProcessor<ScopeData::Samples>>
    {
        if (config.active)
        {
            switch (config.busType)
            {
                case BusType::Mono:
                    return std::make_unique<Generator<StereoChannel::Left>>(
                            args);

                case BusType::Stereo:
                    switch (config.channel)
                    {
                        case StereoChannel::Left:
                            return std::make_unique<
                                    Generator<StereoChannel::Left>>(args);

                        case StereoChannel::Right:
                            return std::make_unique<
                                    Generator<StereoChannel::Right>>(args);

                        case StereoChannel::Middle:
                            return std::make_unique<
                                    Generator<StereoChannel::Middle>>(args);

                        case StereoChannel::Side:
                            return std::make_unique<
                                    Generator<StereoChannel::Side>>(args);

                        default:
                            BOOST_ASSERT_MSG(false, "Unknown StereoChannel");
                            return std::make_unique<InactiveGenerator>(args);
                    }
            }
        }
        else
        {
            return std::make_unique<InactiveGenerator>(args);
        }
    }
};

auto
findTrigger(
        ScopeData::Samples const& samples,
        std::size_t windowSize,
        TriggerSlope trigger,
        float triggerLevel) -> ScopeData::Samples::iterator
{
    if (samples.size() < windowSize)
    {
        return samples.end();
    }

    auto itFirst = samples.begin();
    auto itLast = std::next(samples.begin(), samples.size() - windowSize);

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

enum class ScopeDataGeneratorState
{
    WaitingForTrigger,
    Hold
};

} // namespace

struct ScopeDataGenerator::Impl
{
    Impl(std::span<BusType const> substreamConfigs)
        : streamProcessor{std::move(substreamConfigs)}
    {
    }

    audio::sample_rate sampleRate;
    std::size_t windowSize{64};
    std::chrono::milliseconds holdTime{16};
    std::size_t triggerStream{};
    TriggerSlope triggerSlope{TriggerSlope::RisingEdge};
    float triggerLevel{};
    bool freeze{};
    ScopeDataGeneratorState state{ScopeDataGeneratorState::WaitingForTrigger};

    StreamProcessor<Args, ScopeData::Samples, Factory> streamProcessor;
};

ScopeDataGenerator::ScopeDataGenerator(
        std::span<BusType const> substreamConfigs)
    : m_impl(std::make_unique<Impl>(std::move(substreamConfigs)))
{
}

ScopeDataGenerator::~ScopeDataGenerator() = default;

void
ScopeDataGenerator::setSampleRate(audio::sample_rate const sampleRate)
{
    m_impl->sampleRate = sampleRate;
}

void
ScopeDataGenerator::setResolution(std::size_t const resolution)
{
    BOOST_ASSERT(in_closed(resolution, std::size_t{1}, std::size_t{9}));
    m_impl->streamProcessor.updateProperty(
            m_impl->streamProcessor.args.resolution,
            resolution);
}

void
ScopeDataGenerator::setWindowSize(std::size_t const windowSize)
{
    m_impl->windowSize = windowSize;
}

void
ScopeDataGenerator::setHoldTime(std::chrono::milliseconds const holdTime)
{
    m_impl->holdTime = holdTime;
}

void
ScopeDataGenerator::setTriggerStream(std::size_t const substreamIndex)
{
    BOOST_ASSERT(substreamIndex < m_impl->streamProcessor.configs.size());
    m_impl->triggerStream = substreamIndex;
}

void
ScopeDataGenerator::setTriggerSlope(TriggerSlope const triggerSlope)
{
    m_impl->triggerSlope = triggerSlope;
}

void
ScopeDataGenerator::setTriggerLevel(float const triggerLevel)
{
    m_impl->triggerLevel = triggerLevel;
}

void
ScopeDataGenerator::setActive(
        std::size_t const substreamIndex,
        bool const active)
{
    m_impl->streamProcessor.setActive(substreamIndex, active);
}

void
ScopeDataGenerator::setChannel(
        std::size_t const substreamIndex,
        StereoChannel const channel)
{
    m_impl->streamProcessor.setStereoChannel(substreamIndex, channel);
}

void
ScopeDataGenerator::setFreeze(bool const freeze)
{
    m_impl->freeze = freeze;

    if (!freeze)
    {
        clear();
    }
}

void
ScopeDataGenerator::update(AudioStream const& stream)
{
    if (m_impl->freeze)
    {
        return;
    }

    m_impl->streamProcessor.process(stream);

    auto const triggerStreamSamples =
            m_impl->streamProcessor.results[m_impl->triggerStream];
    auto const capturedSize = triggerStreamSamples.size();
    auto const windowSize = m_impl->windowSize;

    switch (m_impl->state)
    {
        case ScopeDataGeneratorState::WaitingForTrigger:
        {
            auto it = findTrigger(
                    triggerStreamSamples,
                    windowSize,
                    m_impl->triggerSlope,
                    m_impl->triggerLevel);
            if (it != triggerStreamSamples.end())
            {
                auto offset = std::distance(triggerStreamSamples.begin(), it);
                auto result = algorithm::transform_to_vector(
                        m_impl->streamProcessor.results,
                        [=](ScopeData::Samples const& stream) {
                            return ScopeData::Samples{
                                    stream.data() + offset,
                                    windowSize};
                        });

                emit generated(result);

                m_impl->streamProcessor.clear();
                m_impl->state = ScopeDataGeneratorState::Hold;
            }
            else if (capturedSize > windowSize * 2)
            {
                m_impl->streamProcessor.drop(capturedSize - windowSize);
            }
            break;
        }

        case ScopeDataGeneratorState::Hold:
        {
            auto const holdTimeSize =
                    m_impl->sampleRate.to_samples(m_impl->holdTime);
            if (capturedSize >= holdTimeSize)
            {
                m_impl->streamProcessor.clear();
                m_impl->state = ScopeDataGeneratorState::WaitingForTrigger;
            }
            break;
        }

        default:
            break;
    }
}

void
ScopeDataGenerator::clear()
{
    m_impl->streamProcessor.clear();
}

} // namespace piejam::gui::model
