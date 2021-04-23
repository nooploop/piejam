// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StereoSpectrumDataGenerator.h>

#include <boost/assert.hpp>

#include <optional>
#include <vector>

namespace piejam::gui::model
{

namespace
{

struct InactiveGenerator
{
    auto operator()(AudioStreamListener::Stream const&)
            -> std::optional<SpectrumDataPoints>
    {
        return SpectrumDataPoints();
    }
};

template <StereoChannel>
struct Generator;

template <>
struct Generator<StereoChannel::Left>
{
    auto operator()(AudioStreamListener::Stream const&)
            -> std::optional<SpectrumDataPoints>
    {
        return dataPoints;
    }

    std::vector<SpectrumDataPoint> dataPoints{
            SpectrumDataPoint{.frequency_Hz = 20.f, .level_dB = -20.f},
            SpectrumDataPoint{.frequency_Hz = 20000.f, .level_dB = -20.f}};
};

template <>
struct Generator<StereoChannel::Right>
{
    auto operator()(AudioStreamListener::Stream const&)
            -> std::optional<SpectrumDataPoints>
    {
        return dataPoints;
    }

    std::vector<SpectrumDataPoint> dataPoints{
            SpectrumDataPoint{.frequency_Hz = 20.f, .level_dB = -40.f},
            SpectrumDataPoint{.frequency_Hz = 20000.f, .level_dB = -40.f}};
};

template <>
struct Generator<StereoChannel::Middle>
{
    auto operator()(AudioStreamListener::Stream const&)
            -> std::optional<SpectrumDataPoints>
    {
        return dataPoints;
    }

    std::vector<SpectrumDataPoint> dataPoints{
            SpectrumDataPoint{.frequency_Hz = 20.f, .level_dB = -60.f},
            SpectrumDataPoint{.frequency_Hz = 20000.f, .level_dB = -60.f}};
};

template <>
struct Generator<StereoChannel::Side>
{
    auto operator()(AudioStreamListener::Stream const&)
            -> std::optional<SpectrumDataPoints>
    {
        return dataPoints;
    }

    std::vector<SpectrumDataPoint> dataPoints{
            SpectrumDataPoint{.frequency_Hz = 20.f, .level_dB = -80.f},
            SpectrumDataPoint{.frequency_Hz = 20000.f, .level_dB = -80.f}};
};

} // namespace

struct StereoSpectrumDataGenerator::Impl
{
    bool active{};
    StereoChannel channel{StereoChannel::Left};
    std::function<std::optional<SpectrumDataPoints>(
            AudioStreamListener::Stream const&)>
            generator{Generator<StereoChannel::Left>{}};

    void updateGenerator()
    {
        if (active)
        {
            switch (channel)
            {
                case StereoChannel::Left:
                    generator = Generator<StereoChannel::Left>{};
                    break;

                case StereoChannel::Right:
                    generator = Generator<StereoChannel::Right>{};
                    break;

                case StereoChannel::Middle:
                    generator = Generator<StereoChannel::Middle>{};
                    break;

                case StereoChannel::Side:
                    generator = Generator<StereoChannel::Side>{};
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
StereoSpectrumDataGenerator::setActive(bool const active)
{
    m_impl->active = active;
    m_impl->updateGenerator();
}

void
StereoSpectrumDataGenerator::setChannel(StereoChannel const channel)
{
    m_impl->channel = channel;
    m_impl->updateGenerator();
}

void
StereoSpectrumDataGenerator::update(Stream const& stream)
{
    if (auto spectrumData = m_impl->generator(stream))
        generated(*spectrumData);
}

} // namespace piejam::gui::model
