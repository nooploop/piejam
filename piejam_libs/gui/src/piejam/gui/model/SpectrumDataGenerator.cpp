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
        m_streamBuffer.clear();
        m_streamBuffer.reserve(stream.num_frames());

        BOOST_ASSERT(
                stream.layout() == audio::multichannel_layout::non_interleaved);
        auto stereoView = stream.channels_cast<2>();
        if constexpr (SC == StereoChannel::Left)
        {
            m_streamBuffer.insert(
                    m_streamBuffer.end(),
                    stereoView.channels()[0].begin(),
                    stereoView.channels()[0].end());
        }
        else if constexpr (SC == StereoChannel::Right)
        {
            m_streamBuffer.insert(
                    m_streamBuffer.end(),
                    stereoView.channels()[1].begin(),
                    stereoView.channels()[1].end());
        }
        else if constexpr (SC == StereoChannel::Middle)
        {
            std::ranges::transform(
                    stereoView.channels()[0],
                    stereoView.channels()[1],
                    std::back_inserter(m_streamBuffer),
                    std::plus{});
        }
        else if constexpr (SC == StereoChannel::Side)
        {
            std::ranges::transform(
                    stereoView.channels()[0],
                    stereoView.channels()[1],
                    std::back_inserter(m_streamBuffer),
                    std::minus{});
        }

        algorithm::shift_push_back(m_dftPrepareBuffer, m_streamBuffer);

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
    std::vector<float> m_streamBuffer{};
    std::vector<float> m_dftPrepareBuffer{std::vector<float>(m_dft.size())};
    std::vector<float> m_window{algorithm::transform_to_vector(
            range::iota(m_dft.size()),
            std::bind_front(&numeric::window::hann, m_dft.size()))};
    std::vector<SpectrumDataPoint> m_dataPoints{
            std::vector<SpectrumDataPoint>(m_dft.output_size())};
};

} // namespace

struct SpectrumDataGenerator::Impl
{
    BusType busType;
    audio::sample_rate sampleRate{48000};
    DFTResolution resolution{DFTResolution::Low};
    bool active{};
    StereoChannel channel{StereoChannel::Left};

    using Generate = std::function<SpectrumDataPoints(
            AudioStreamListener::Stream const&)>;
    Generate generator{InactiveGenerator{}};

    void updateGenerator()
    {
        if (active)
        {
            switch (busType)
            {
                case BusType::Mono:
                    generator = Generator<StereoChannel::Left>{
                            resolution,
                            sampleRate};
                    break;

                case BusType::Stereo:
                    switch (channel)
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
            updateGenerator();
        }
    }
};

SpectrumDataGenerator::SpectrumDataGenerator()
    : m_impl(std::make_unique<Impl>())
{
}

SpectrumDataGenerator::~SpectrumDataGenerator() = default;

void
SpectrumDataGenerator::setBusType(BusType const busType)
{
    m_impl->updateGeneratorProperty(m_impl->busType, busType);
}

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
SpectrumDataGenerator::setActive(bool const active)
{
    m_impl->updateGeneratorProperty(m_impl->active, active);
}

void
SpectrumDataGenerator::setChannel(StereoChannel const channel)
{
    m_impl->updateGeneratorProperty(m_impl->channel, channel);
}

void
SpectrumDataGenerator::update(Stream const& stream)
{
    generated(m_impl->generator(stream));
}

} // namespace piejam::gui::model
