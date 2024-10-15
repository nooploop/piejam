// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerDbScales.h>

#include <piejam/gui/model/DbScaleData.h>

#include <piejam/functional/in_interval.h>
#include <piejam/math.h>
#include <piejam/runtime/fader_mapping.h>

#include <ranges>

namespace piejam::gui::model
{

namespace
{

template <std::ranges::range Boundaries>
constexpr auto
makeScaleTick(Boundaries&& boundaries, float const dB)
{
    auto const lower = std::ranges::adjacent_find(
            std::forward<Boundaries>(boundaries),
            in_closed(dB),
            [](auto b) { return b.dB; });
    BOOST_ASSERT(lower != std::ranges::end(boundaries));
    auto const upper = std::next(lower);

    return DbScaleTick{
            .normalized = math::linear_map(
                    dB,
                    lower->dB,
                    upper->dB,
                    lower->normalized,
                    upper->normalized),
            .dB = dB,
    };
}

inline constexpr DbScaleTick infinityTick{
        .normalized = 0.f,
        .dB = -std::numeric_limits<float>::infinity()};

constexpr auto
makeLevelMeterScaleTick(float const dB) noexcept -> DbScaleTick
{
    return DbScaleTick{
            .normalized = math::linear_map(dB, -60.f, 0.f, 0.05f, 1.f),
            .dB = dB};
}

constexpr auto
makeVolumeFaderScaleTick(float const dB) noexcept -> DbScaleTick
{
    return makeScaleTick(runtime::fader_mapping::volume, dB);
}

constexpr auto
makeSendFaderScaleTick(float const dB) noexcept -> DbScaleTick
{
    return makeScaleTick(runtime::fader_mapping::send, dB);
}

} // namespace

MixerDbScales::MixerDbScales()
    : m_levelMeterScale(std::make_unique<DbScaleData>(QVector<DbScaleTick>({
              infinityTick,
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
              infinityTick,
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
              infinityTick,
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
