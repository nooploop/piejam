// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/WaveformDataGenerator.h>

#include <piejam/gui/model/StreamProcessor.h>
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

struct InactiveGenerator final : SubStreamProcessor<WaveformData>
{
    auto process(AudioStream const&) -> WaveformData override
    {
        return WaveformData{};
    }

    void clear() override
    {
    }
};

[[nodiscard]]
constexpr auto
clip(float const v) -> float
{
    return math::clamp(v, -1.f, 1.f);
}

template <StereoChannel SC>
class Generator final : public SubStreamProcessor<WaveformData>
{
public:
    explicit Generator(int const samplesPerPixel)
        : m_samplesPerPixel{samplesPerPixel}
    {
    }

    auto process(AudioStream const& stream) -> WaveformData override
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

            if (m_accNumSamples >= m_samplesPerPixel)
            {
                result.push_back(clip(m_accY0), clip(m_accY1));

                m_accNumSamples = 0;
            }
        }

        return result;
    }

    void clear() override
    {
        m_accY0 = 0;
        m_accY1 = 0;
        m_accNumSamples = 0;
    }

private:
    int m_samplesPerPixel{1};

    float m_accY0{};
    float m_accY1{};
    int m_accNumSamples{};
};

struct Args
{
    int samplesPerPixel{1};
};

struct Factory
{
    auto operator()(SubStreamProcessorConfig const& config, Args const& args)
            -> std::unique_ptr<SubStreamProcessor<WaveformData>>
    {
        if (config.active)
        {
            switch (config.busType)
            {
                case BusType::Mono:
                    return std::make_unique<Generator<StereoChannel::Left>>(
                            args.samplesPerPixel);

                case BusType::Stereo:
                    switch (config.channel)
                    {
                        case StereoChannel::Left:
                            return std::make_unique<
                                    Generator<StereoChannel::Left>>(
                                    args.samplesPerPixel);

                        case StereoChannel::Right:
                            return std::make_unique<
                                    Generator<StereoChannel::Right>>(
                                    args.samplesPerPixel);

                        case StereoChannel::Middle:
                            return std::make_unique<
                                    Generator<StereoChannel::Middle>>(
                                    args.samplesPerPixel);

                        case StereoChannel::Side:
                            return std::make_unique<
                                    Generator<StereoChannel::Side>>(
                                    args.samplesPerPixel);

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

struct WaveformDataGenerator::Impl
{
    explicit Impl(std::span<BusType const> substreamConfigs)
        : streamProcessor{substreamConfigs}
    {
    }

    StreamProcessor<Args, WaveformData, Factory> streamProcessor;
    bool freeze{};
};

WaveformDataGenerator::WaveformDataGenerator(
        std::span<BusType const> substreamConfigs)
    : m_impl{make_pimpl<Impl>(substreamConfigs)}
{
}

void
WaveformDataGenerator::setSamplesPerPixel(int const samplesPerPixel)
{
    if (samplesPerPixel <= 0)
    {
        throw std::invalid_argument("samplesPerPixel must be positive");
    }

    m_impl->streamProcessor.updateProperty(
            m_impl->streamProcessor.args.samplesPerPixel,
            samplesPerPixel);
}

void
WaveformDataGenerator::setActive(
        std::size_t const substreamIndex,
        bool const active)
{
    m_impl->streamProcessor.setActive(substreamIndex, active);
}

void
WaveformDataGenerator::setChannel(
        std::size_t const substreamIndex,
        StereoChannel const channel)
{
    m_impl->streamProcessor.setStereoChannel(substreamIndex, channel);
}

void
WaveformDataGenerator::setFreeze(bool const freeze)
{
    m_impl->freeze = freeze;

    if (!freeze)
    {
        clear();
    }
}

void
WaveformDataGenerator::clear()
{
    m_impl->streamProcessor.clear();
}

void
WaveformDataGenerator::update(AudioStream stream)
{
    if (m_impl->freeze)
    {
        return;
    }

    m_impl->streamProcessor.process(stream);

    emit generated(m_impl->streamProcessor.results);
}

} // namespace piejam::gui::model
