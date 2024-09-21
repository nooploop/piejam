// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerDbScales.h>

#include <piejam/gui/model/DbScaleData.h>

#include <piejam/functional/in_interval.h>
#include <piejam/math.h>

#include <array>
#include <ranges>

namespace piejam::gui::model
{

static constexpr auto s_volumeFaderScaleTickBoundaries = std::array{
        DbScaleTick{.position = 0.05f, .dB = -60.f},
        DbScaleTick{.position = 0.3f, .dB = -30.f},
        DbScaleTick{.position = 0.55f, .dB = -12.f},
        DbScaleTick{.position = 1.f, .dB = 6.f},
};

static constexpr auto s_sendFaderScaleTickBoundaries = std::array{
        DbScaleTick{.position = 0.05f, .dB = -60.f},
        DbScaleTick{.position = 0.3f, .dB = -30.f},
        DbScaleTick{.position = 0.48f, .dB = -18.f},
        DbScaleTick{.position = 1.f, .dB = 0.f},
};

template <std::ranges::range Boundaries>
static constexpr auto
makeScaleTick(Boundaries&& boundaries, float const dB)
{
    auto const lower = std::ranges::adjacent_find(
            std::forward<Boundaries>(boundaries),
            in_closed(dB),
            &DbScaleTick::dB);
    BOOST_ASSERT(lower != std::ranges::end(boundaries));
    auto const upper = std::next(lower);

    return DbScaleTick{
            .position = math::linear_map(
                    dB,
                    lower->dB,
                    upper->dB,
                    lower->position,
                    upper->position),
            .dB = dB,
    };
}

static constexpr auto
makeLevelMeterScaleTick(float const dB) noexcept -> DbScaleTick
{
    return DbScaleTick{
            .position = math::linear_map(dB, -60.f, 0.f, 0.05f, 1.f),
            .dB = dB};
}

static constexpr auto
makeVolumeFaderScaleTick(float const dB) noexcept -> DbScaleTick
{
    return makeScaleTick(s_volumeFaderScaleTickBoundaries, dB);
}

static constexpr auto
makeSendFaderScaleTick(float const dB) noexcept -> DbScaleTick
{
    return makeScaleTick(s_sendFaderScaleTickBoundaries, dB);
}

MixerDbScales::MixerDbScales()
    : m_levelMeterScale(std::make_unique<DbScaleData>(QVector<DbScaleTick>({
              DbScaleTick{
                      .position = 0.f,
                      .dB = -std::numeric_limits<float>::infinity()},
              makeLevelMeterScaleTick(-60.f),
              makeLevelMeterScaleTick(-54.f),
              makeLevelMeterScaleTick(-48.f),
              makeLevelMeterScaleTick(-42.f),
              makeLevelMeterScaleTick(-36.f),
              makeLevelMeterScaleTick(-30.f),
              makeLevelMeterScaleTick(-24.f),
              makeLevelMeterScaleTick(-18.f),
              makeLevelMeterScaleTick(-12.f),
              makeLevelMeterScaleTick(-6.f),
              makeLevelMeterScaleTick(0.f),
      })))
    , m_volumeFaderScale(std::make_unique<DbScaleData>(QVector<DbScaleTick>({
              DbScaleTick{
                      .position = 0.f,
                      .dB = -std::numeric_limits<float>::infinity()},
              makeVolumeFaderScaleTick(-60.f),
              makeVolumeFaderScaleTick(-50.f),
              makeVolumeFaderScaleTick(-40.f),
              makeVolumeFaderScaleTick(-30.f),
              makeVolumeFaderScaleTick(-24.f),
              makeVolumeFaderScaleTick(-18.f),
              makeVolumeFaderScaleTick(-12.f),
              makeVolumeFaderScaleTick(-9.f),
              makeVolumeFaderScaleTick(-6.f),
              makeVolumeFaderScaleTick(-3.f),
              makeVolumeFaderScaleTick(0.f),
              makeVolumeFaderScaleTick(3.f),
              makeVolumeFaderScaleTick(6.f),
      })))
    , m_sendFaderScale(std::make_unique<DbScaleData>(QVector<DbScaleTick>({
              DbScaleTick{
                      .position = 0.f,
                      .dB = -std::numeric_limits<float>::infinity()},
              makeSendFaderScaleTick(-60.f),
              makeSendFaderScaleTick(-50.f),
              makeSendFaderScaleTick(-40.f),
              makeSendFaderScaleTick(-30.f),
              makeSendFaderScaleTick(-24.f),
              makeSendFaderScaleTick(-18.f),
              makeSendFaderScaleTick(-15.f),
              makeSendFaderScaleTick(-12.f),
              makeSendFaderScaleTick(-9.f),
              makeSendFaderScaleTick(-6.f),
              makeSendFaderScaleTick(-3.f),
              makeSendFaderScaleTick(0.f),
      })))
{
}

MixerDbScales::~MixerDbScales() = default;

MixerDbScales g_mixerDbScales;

} // namespace piejam::gui::model
