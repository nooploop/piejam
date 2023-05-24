// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/SpectrumDataGenerator.h>

#include <piejam/algorithm/shift_push_back.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/math.h>
#include <piejam/numeric/dft.h>
#include <piejam/numeric/window.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>

#include <boost/assert.hpp>

#include <vector>

namespace piejam::gui::model
{

namespace
{

auto
dftForResolution(DFTResolution const resolution) -> numeric::dft&
{
    switch (resolution)
    {
        case DFTResolution::Middle:
        {
            static numeric::dft s_dft{4096};
            return s_dft;
        }

        case DFTResolution::High:
        {
            static numeric::dft s_dft{8192};
            return s_dft;
        }

        case DFTResolution::VeryHigh:
        {
            static numeric::dft s_dft{16384};
            return s_dft;
        }

        case DFTResolution::Low:
        default:
        {
            static numeric::dft s_dft{2048};
            return s_dft;
        }
    }
}

struct InactiveGenerator
{
    auto operator()(AudioStreamListener::Stream const&) -> SpectrumDataPoints
    {
        return SpectrumDataPoints();
    }
};

template <StereoChannel SC>
class Generator
{
public:
    Generator(DFTResolution resolution, audio::sample_rate const sampleRate)
        : m_dft{dftForResolution(resolution)}
    {
        float const binSize = sampleRate.as_float() / m_dft.size();
        for (std::size_t const i : range::iota(m_dft.output_size()))
        {
            m_dataPoints[i].frequency_Hz = i * binSize;
        }
    }

    static constexpr auto envelope(float const prev, float const in) noexcept
            -> float
    {
        BOOST_ASSERT(in >= 0.f);
        return in > prev ? in : in + 0.85f * (prev - in);
    }

    auto operator()(AudioStreamListener::Stream const& stream)
            -> SpectrumDataPoints
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

        algorithm::shift_push_back(
                m_dftPrepareBuffer,
                streamFrames | std::views::transform(StereoFrameValue<SC>{}));

        std::transform(
                m_dftPrepareBuffer.begin(),
                m_dftPrepareBuffer.end(),
                m_window.begin(),
                m_dft.input_buffer().begin(),
                std::multiplies<>{});

        auto const spectrum = m_dft.process();

        BOOST_ASSERT(spectrum.size() == m_dft.output_size());
        BOOST_ASSERT(m_dataPoints.size() == m_dft.output_size());
        for (std::size_t const i : range::iota(m_dft.output_size()))
        {
            m_dataPoints[i].level_linear = envelope(
                    m_dataPoints[i].level_linear,
                    std::abs(spectrum[i]) * (2.f / m_dft.size()));
            m_dataPoints[i].level_dB =
                    math::linear_to_dB(m_dataPoints[i].level_linear);
        }

        return m_dataPoints;
    }

private:
    numeric::dft& m_dft;
    std::vector<float> m_dftPrepareBuffer{std::vector<float>(m_dft.size())};
    std::vector<float> m_window{algorithm::transform_to_vector(
            range::iota(m_dft.size()),
            std::bind_front(&numeric::window::hann, m_dft.size()))};
    std::vector<SpectrumDataPoint> m_dataPoints{
            std::vector<SpectrumDataPoint>(m_dft.output_size())};
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

struct SpectrumDataGenerator::Impl
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

    void updateGenerator(std::size_t const substreamIndex)
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
                    generator = Generator<StereoChannel::Left>{
                            resolution,
                            sampleRate};
                    break;

                case BusType::Stereo:
                    switch (config.channel)
                    {
                        case StereoChannel::Left:
                            generator = Generator<StereoChannel::Left>{
                                    resolution,
                                    sampleRate};
                            break;

                        case StereoChannel::Right:
                            generator = Generator<StereoChannel::Right>{
                                    resolution,
                                    sampleRate};
                            break;

                        case StereoChannel::Middle:
                            generator = Generator<StereoChannel::Middle>{
                                    resolution,
                                    sampleRate};
                            break;

                        case StereoChannel::Side:
                            generator = Generator<StereoChannel::Side>{
                                    resolution,
                                    sampleRate};
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

    using Generate = std::function<SpectrumDataPoints(
            AudioStreamListener::Stream const&)>;

    std::vector<Generate> generators;
    std::vector<SpectrumDataPoints> results;
    std::vector<GeneratorConfig> generatorConfigs;

    audio::sample_rate sampleRate{48000};
    DFTResolution resolution{DFTResolution::Low};
};

SpectrumDataGenerator::SpectrumDataGenerator(
        std::span<BusType const> substreamConfigs)
    : m_impl(std::make_unique<Impl>(std::move(substreamConfigs)))
{
}

SpectrumDataGenerator::~SpectrumDataGenerator() = default;

void
SpectrumDataGenerator::setSampleRate(audio::sample_rate const sampleRate)
{
    m_impl->updateGeneratorProperty(m_impl->sampleRate, sampleRate);
}

void
SpectrumDataGenerator::setResolution(DFTResolution const resolution)
{
    m_impl->updateGeneratorProperty(m_impl->resolution, resolution);
}

void
SpectrumDataGenerator::setActive(
        std::size_t const substreamIndex,
        bool const active)
{
    BOOST_ASSERT(substreamIndex < m_impl->generatorConfigs.size());
    m_impl->updateGeneratorProperty(
            m_impl->generatorConfigs[substreamIndex].active,
            active);
}

void
SpectrumDataGenerator::setChannel(
        std::size_t const substreamIndex,
        StereoChannel const channel)
{
    BOOST_ASSERT(substreamIndex < m_impl->generatorConfigs.size());
    m_impl->updateGeneratorProperty(
            m_impl->generatorConfigs[substreamIndex].channel,
            channel);
}

void
SpectrumDataGenerator::update(Stream const& stream)
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
