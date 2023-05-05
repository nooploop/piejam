// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/SpectrumDataGenerator.h>

#include <piejam/algorithm/shift_push_back.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/functional/operators.h>
#include <piejam/math.h>
#include <piejam/numeric/dft.h>
#include <piejam/numeric/window.h>
#include <piejam/range/iota.h>

#include <boost/assert.hpp>

#include <optional>
#include <vector>

namespace piejam::gui::model
{

namespace
{

constexpr std::size_t s_dft_size{2048};
numeric::dft s_dft{s_dft_size};

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
    Generator(audio::sample_rate const sampleRate)
    {
        float const binSize = sampleRate.as_float() / s_dft_size;
        for (std::size_t const i : range::iota(s_dft.output_size()))
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
                m_dftInputBuffer.begin(),
                std::multiplies<>{});

        auto const spectrum = s_dft.process(m_dftInputBuffer);

        BOOST_ASSERT(spectrum.size() == s_dft.output_size());
        BOOST_ASSERT(m_dataPoints.size() == s_dft.output_size());
        for (std::size_t const i : range::iota(s_dft.output_size()))
        {
            m_dataPoints[i].level_linear = envelope(
                    m_dataPoints[i].level_linear,
                    std::abs(spectrum[i]) * (2.f / s_dft_size));
            m_dataPoints[i].level_dB =
                    math::linear_to_dB(m_dataPoints[i].level_linear);
        }

        return m_dataPoints;
    }

private:
    std::vector<float> m_streamBuffer{};
    std::vector<float> m_dftPrepareBuffer{std::vector<float>(s_dft_size)};
    std::vector<float> m_dftInputBuffer{std::vector<float>(s_dft_size)};
    std::vector<float> m_window{algorithm::transform_to_vector(
            range::iota(s_dft_size),
            &numeric::window::hann<s_dft_size>)};
    std::vector<SpectrumDataPoint> m_dataPoints{
            std::vector<SpectrumDataPoint>(s_dft.output_size())};
};

} // namespace

struct SpectrumDataGenerator::Impl
{
    BusType busType;
    audio::sample_rate sampleRate{48000};
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
                    generator = Generator<StereoChannel::Left>{sampleRate};
                    break;

                case BusType::Stereo:
                    switch (channel)
                    {
                        case StereoChannel::Left:
                            generator =
                                    Generator<StereoChannel::Left>{sampleRate};
                            break;

                        case StereoChannel::Right:
                            generator =
                                    Generator<StereoChannel::Right>{sampleRate};
                            break;

                        case StereoChannel::Middle:
                            generator = Generator<StereoChannel::Middle>{
                                    sampleRate};
                            break;

                        case StereoChannel::Side:
                            generator =
                                    Generator<StereoChannel::Side>{sampleRate};
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
