// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/PitchGenerator.h>

#include <piejam/gui/model/StreamProcessor.h>

#include <piejam/algorithm/shift_push_back.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/dsp/pitch_yin.h>
#include <piejam/audio/dsp/rms.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/math.h>

#include <boost/assert.hpp>

#include <vector>

namespace piejam::gui::model
{

namespace
{

struct InactiveGenerator final : SubStreamProcessor<float>
{
    auto process(AudioStream const&) -> float override
    {
        return 0.f;
    }

    void clear() override
    {
    }
};

struct Args
{
    audio::sample_rate sampleRate{48000};
};

template <StereoChannel SC>
class Generator final : public SubStreamProcessor<float>
{
    static constexpr std::size_t windowSize{8192};
    static constexpr std::size_t captureSize{4096};

public:
    explicit Generator(Args const& args)
        : m_signal(windowSize)
        , m_sr{args.sampleRate}
    {
    }

    auto process(AudioStream const& stream) -> float override
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
                m_signal,
                streamFrames | std::views::transform(StereoFrameValue<SC>{}));

        m_capturedFrames += stream.num_frames();
        if (m_capturedFrames >= captureSize)
        {
            if (math::to_dB(audio::dsp::simd::rms<float>(m_signal)) < -60.f)
            {
                m_lastFrequency = 0.f;
            }
            else
            {
                m_lastFrequency = audio::dsp::pitch_yin<float>(m_signal, m_sr);
            }

            m_capturedFrames %= captureSize;
        }

        return m_lastFrequency;
    }

    void clear() override
    {
        std::ranges::fill(m_signal, 0.f);
    }

private:
    std::vector<float> m_signal;
    std::size_t m_capturedFrames{};
    float m_lastFrequency{};
    audio::sample_rate m_sr;
};

struct Factory
{
    auto operator()(SubStreamProcessorConfig const& config, Args const& args)
            -> std::unique_ptr<SubStreamProcessor<float>>
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

                        default:
                            BOOST_ASSERT_MSG(
                                    false,
                                    "Unsupported StereoChannel");
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

struct PitchGenerator::Impl
{
    explicit Impl(std::span<BusType const> substreamConfigs)
        : streamProcessor{substreamConfigs}
    {
    }

    StreamProcessor<Args, float, Factory> streamProcessor;
};

PitchGenerator::PitchGenerator(std::span<BusType const> substreamConfigs)
    : m_impl{std::make_unique<Impl>(substreamConfigs)}
{
}

PitchGenerator::~PitchGenerator() = default;

void
PitchGenerator::setSampleRate(audio::sample_rate const sampleRate)
{
    m_impl->streamProcessor.updateProperty(
            m_impl->streamProcessor.args.sampleRate,
            sampleRate);
}

void
PitchGenerator::setActive(std::size_t const substreamIndex, bool const active)
{
    m_impl->streamProcessor.setActive(substreamIndex, active);
}

void
PitchGenerator::setChannel(
        std::size_t const substreamIndex,
        StereoChannel const channel)
{
    m_impl->streamProcessor.setStereoChannel(substreamIndex, channel);
}

void
PitchGenerator::update(AudioStream const& stream)
{
    m_impl->streamProcessor.process(stream);
    BOOST_ASSERT(m_impl->streamProcessor.results.size() == 1);
    emit generated(m_impl->streamProcessor.results[0]);
}

} // namespace piejam::gui::model
