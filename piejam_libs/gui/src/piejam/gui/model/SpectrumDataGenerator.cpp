// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/SpectrumDataGenerator.h>

#include <piejam/gui/model/StreamProcessor.h>

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
        case DFTResolution::Medium:
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

struct InactiveGenerator final : SubStreamProcessor<SpectrumDataPoints>
{
    auto process(AudioStream const&) -> SpectrumDataPoints override
    {
        return {};
    }

    void clear() override
    {
    }
};

struct Args
{
    audio::sample_rate sampleRate{48000};
    DFTResolution resolution{DFTResolution::Low};
};

template <StereoChannel SC>
class Generator final : public SubStreamProcessor<SpectrumDataPoints>
{
public:
    explicit Generator(Args const& args)
        : m_dft{dftForResolution(args.resolution)}
    {
        float const binSize = args.sampleRate.as_float() / m_dft.size();
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

    auto process(AudioStream const& stream) -> SpectrumDataPoints override
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
            m_dataPoints[i].level = envelope(
                    m_dataPoints[i].level,
                    std::abs(spectrum[i]) * (2.f / m_dft.size()));
            m_dataPoints[i].level_dB = math::to_dB(m_dataPoints[i].level);
        }

        return m_dataPoints;
    }

    void clear() override
    {
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

struct Factory
{
    auto operator()(SubStreamProcessorConfig const& config, Args const& args)
            -> std::unique_ptr<SubStreamProcessor<SpectrumDataPoints>>
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
                            return std::make_unique<InactiveGenerator>();
                    }
            }
        }
        else
        {
            return std::make_unique<InactiveGenerator>();
        }
    }
};

} // namespace

struct SpectrumDataGenerator::Impl
{
    explicit Impl(std::span<BusType const> substreamConfigs)
        : streamProcessor{substreamConfigs}
    {
    }

    StreamProcessor<Args, SpectrumDataPoints, Factory> streamProcessor;
    bool freeze{};
};

SpectrumDataGenerator::SpectrumDataGenerator(
        std::span<BusType const> substreamConfigs)
    : m_impl{std::make_unique<Impl>(substreamConfigs)}
{
}

SpectrumDataGenerator::~SpectrumDataGenerator() = default;

void
SpectrumDataGenerator::setSampleRate(audio::sample_rate const sampleRate)
{
    m_impl->streamProcessor.updateProperty(
            m_impl->streamProcessor.args.sampleRate,
            sampleRate);
}

void
SpectrumDataGenerator::setResolution(DFTResolution const resolution)
{
    m_impl->streamProcessor.updateProperty(
            m_impl->streamProcessor.args.resolution,
            resolution);
}

void
SpectrumDataGenerator::setActive(
        std::size_t const substreamIndex,
        bool const active)
{
    m_impl->streamProcessor.setActive(substreamIndex, active);
}

void
SpectrumDataGenerator::setChannel(
        std::size_t const substreamIndex,
        StereoChannel const channel)
{
    m_impl->streamProcessor.setStereoChannel(substreamIndex, channel);
}

void
SpectrumDataGenerator::setFreeze(bool const freeze)
{
    m_impl->freeze = freeze;

    if (!freeze)
    {
        m_impl->streamProcessor.clear();
    }
}

void
SpectrumDataGenerator::update(AudioStream const& stream)
{
    if (m_impl->freeze)
    {
        return;
    }

    m_impl->streamProcessor.process(stream);
    emit generated(m_impl->streamProcessor.results);
}

} // namespace piejam::gui::model
