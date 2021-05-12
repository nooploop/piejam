// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StereoSpectrumDataGenerator.h>

#include <piejam/algorithm/shift_push_back.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/math.h>
#include <piejam/numeric/dft.h>
#include <piejam/numeric/window.h>
#include <piejam/range/interleaved_view.h>
#include <piejam/range/iota.h>

#include <boost/assert.hpp>

#include <optional>
#include <vector>

namespace piejam::gui::model
{

namespace
{

constexpr std::size_t s_dft_size{1024};
numeric::dft s_dft{s_dft_size};

struct InactiveGenerator
{
    auto operator()(AudioStreamListener::Stream const&)
            -> std::optional<SpectrumDataPoints>
    {
        return SpectrumDataPoints();
    }
};

template <StereoChannel SC>
class Generator
{
public:
    Generator(unsigned const m_sampleRate)
        : m_sampleRate(static_cast<float>(m_sampleRate))
        , m_window(algorithm::transform_to_vector(
                  range::iota(s_dft_size),
                  &numeric::window::hamming<s_dft_size>))
    {
        for (std::size_t const i : range::iota(s_dft.output_size()))
            m_dataPoints[i].frequency_Hz = i * m_binSize;
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

    static constexpr auto envelope(float const prev, float const in) noexcept
            -> float
    {
        BOOST_ASSERT(in >= 0.f);
        return in > prev ? in : in + 0.85f * (prev - in);
    }

    auto operator()(AudioStreamListener::Stream const& stream)
            -> std::optional<SpectrumDataPoints>
    {
        auto const numFrames = std::distance(stream.begin(), stream.end());

        m_streamBuffer.clear();
        m_streamBuffer.reserve(numFrames);

        std::ranges::transform(
                stream.channels_cast<2>(),
                std::back_inserter(m_streamBuffer),
                &frameValue);

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
                    std::abs(spectrum[i]) * 2.f / s_dft_size);
            m_dataPoints[i].level_dB =
                    math::linear_to_dB(m_dataPoints[i].level_linear);
        }

        return m_dataPoints;
    }

private:
    float m_sampleRate{48000};
    float m_binSize{m_sampleRate / s_dft_size};

    std::vector<float> m_streamBuffer{};
    std::vector<float> m_dftPrepareBuffer{std::vector<float>(s_dft_size)};
    std::vector<float> m_dftInputBuffer{std::vector<float>(s_dft_size)};
    std::vector<float> m_window{};
    std::vector<SpectrumDataPoint> m_dataPoints{
            std::vector<SpectrumDataPoint>(s_dft.output_size())};
};

} // namespace

struct StereoSpectrumDataGenerator::Impl
{
    unsigned sampleRate{48000};
    bool active{};
    StereoChannel channel{StereoChannel::Left};
    std::function<std::optional<SpectrumDataPoints>(
            AudioStreamListener::Stream const&)>
            generator{Generator<StereoChannel::Left>{sampleRate}};

    void updateGenerator()
    {
        if (active)
        {
            switch (channel)
            {
                case StereoChannel::Left:
                    generator = Generator<StereoChannel::Left>{sampleRate};
                    break;

                case StereoChannel::Right:
                    generator = Generator<StereoChannel::Right>{sampleRate};
                    break;

                case StereoChannel::Middle:
                    generator = Generator<StereoChannel::Middle>{sampleRate};
                    break;

                case StereoChannel::Side:
                    generator = Generator<StereoChannel::Side>{sampleRate};
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

StereoSpectrumDataGenerator::StereoSpectrumDataGenerator()
    : m_impl(std::make_unique<Impl>())
{
}

StereoSpectrumDataGenerator::~StereoSpectrumDataGenerator() = default;

void
StereoSpectrumDataGenerator::setSampleRate(unsigned const sampleRate)
{
    if (m_impl->sampleRate != sampleRate)
    {
        m_impl->sampleRate = sampleRate;
        m_impl->updateGenerator();
    }
}

void
StereoSpectrumDataGenerator::setActive(bool const active)
{
    if (m_impl->active != active)
    {
        m_impl->active = active;
        m_impl->updateGenerator();
    }
}

void
StereoSpectrumDataGenerator::setChannel(StereoChannel const channel)
{
    if (m_impl->channel != channel)
    {
        m_impl->channel = channel;
        m_impl->updateGenerator();
    }
}

void
StereoSpectrumDataGenerator::update(Stream const& stream)
{
    if (auto spectrumData = m_impl->generator(stream))
        generated(*spectrumData);
}

} // namespace piejam::gui::model
