// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/WaveformDataGenerator.h>

#include <piejam/gui/model/WaveformData.h>

#include <piejam/audio/multichannel_view.h>
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
    auto operator()(AudioStreamListener::Stream const&) -> WaveformData
    {
        return WaveformData{};
    }
};

[[nodiscard]] constexpr auto
clip(float const v) -> float
{
    return math::clamp(v, -1.f, 1.f);
}

template <StereoChannel SC>
class Generator
{
public:
    Generator(int const samplesPerPoint)
        : m_samplesPerPoint{samplesPerPoint}
    {
    }

    auto operator()(AudioStreamListener::Stream const& stream) -> WaveformData
    {
        WaveformData result;

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

        for (auto const sample :
             streamFrames | std::views::transform(StereoFrameValue<SC>{}))
        {
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

struct GeneratorConfig
{
    BusType busType;
    bool active{false};
    StereoChannel channel{StereoChannel::Left};

    // substream info
    std::size_t startChannel{};
    std::size_t numChannels{};
};

} // namespace

struct WaveformDataGenerator::Impl
{
    Impl(std::span<BusType const> substreamConfigs)
        : generators(substreamConfigs.size(), InactiveGenerator{})
        , results(substreamConfigs.size())
    {
        std::size_t startChannel{};
        for (BusType const busType : substreamConfigs)
        {
            auto& config =
                    generatorConfigs.emplace_back(GeneratorConfig{busType});
            config.startChannel = startChannel;
            config.numChannels = num_channels(toBusType(busType));

            startChannel += config.numChannels;
        }
    }

    void updateGenerator(std::size_t substreamIndex)
    {
        BOOST_ASSERT(substreamIndex < generatorConfigs.size());
        auto const& config = generatorConfigs[substreamIndex];

        BOOST_ASSERT(substreamIndex < generators.size());
        auto& generator = generators[substreamIndex];

        if (config.active)
        {
            switch (config.busType)
            {
                case BusType::Mono:
                    generator = Generator<StereoChannel::Left>{samplesPerLine};
                    break;

                case BusType::Stereo:
                    switch (config.channel)
                    {
                        case StereoChannel::Left:
                            generator = Generator<StereoChannel::Left>{
                                    samplesPerLine};
                            break;

                        case StereoChannel::Right:
                            generator = Generator<StereoChannel::Right>{
                                    samplesPerLine};
                            break;

                        case StereoChannel::Middle:
                            generator = Generator<StereoChannel::Middle>{
                                    samplesPerLine};
                            break;

                        case StereoChannel::Side:
                            generator = Generator<StereoChannel::Side>{
                                    samplesPerLine};
                            break;

                        default:
                            BOOST_ASSERT_MSG(false, "Unknown StereoChannel");
                            generator = InactiveGenerator{};
                            break;
                    }
            }
        }
        else
        {
            generator = InactiveGenerator{};
        }
    }

    template <class T>
    void updateGeneratorProperty(T& member, T value)
    {
        if (member != value)
        {
            member = std::move(value);

            for (std::size_t substreamIndex : range::indices(generatorConfigs))
            {
                updateGenerator(substreamIndex);
            }
        }
    }

    using Generate =
            std::function<WaveformData(AudioStreamListener::Stream const&)>;

    std::vector<Generate> generators;
    std::vector<WaveformData> results;
    std::vector<GeneratorConfig> generatorConfigs;

    int samplesPerLine{1};
};

WaveformDataGenerator::WaveformDataGenerator(
        std::span<BusType const> substreamConfigs)
    : m_impl(std::make_unique<Impl>(std::move(substreamConfigs)))
{
}

WaveformDataGenerator::~WaveformDataGenerator() = default;

void
WaveformDataGenerator::setSamplesPerLine(int const samplesPerLine)
{
    if (samplesPerLine <= 0)
    {
        throw std::invalid_argument("samplesPerPoint must be positive");
    }

    m_impl->updateGeneratorProperty(m_impl->samplesPerLine, samplesPerLine);
}

void
WaveformDataGenerator::setActive(
        std::size_t const substreamIndex,
        bool const active)
{
    BOOST_ASSERT(substreamIndex < m_impl->generatorConfigs.size());
    m_impl->updateGeneratorProperty(
            m_impl->generatorConfigs[substreamIndex].active,
            active);
}

void
WaveformDataGenerator::setStereoChannel(
        std::size_t const substreamIndex,
        StereoChannel const channel)
{
    BOOST_ASSERT(substreamIndex < m_impl->generatorConfigs.size());
    m_impl->updateGeneratorProperty(
            m_impl->generatorConfigs[substreamIndex].channel,
            channel);
}

void
WaveformDataGenerator::clear()
{
    for (std::size_t substreamIndex : range::indices(m_impl->generatorConfigs))
    {
        m_impl->updateGenerator(substreamIndex);
    }
}

void
WaveformDataGenerator::update(Stream const& stream)
{
    for (std::size_t substreamIndex : range::indices(m_impl->generatorConfigs))
    {
        auto const& config = m_impl->generatorConfigs[substreamIndex];

        BOOST_ASSERT(substreamIndex < m_impl->generators.size());
        BOOST_ASSERT(substreamIndex < m_impl->results.size());
        m_impl->results[substreamIndex] =
                m_impl->generators[substreamIndex](stream.channels_subview(
                        config.startChannel,
                        config.numChannels));
    }

    generated(m_impl->results);
}

} // namespace piejam::gui::model
