// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerDbScales.h>

#include <piejam/gui/model/DbScaleData.h>

#include <piejam/math.h>

namespace piejam::gui::model
{

static constexpr auto
makeLevelMeterScaleTick(float const dB) noexcept -> DbScaleTick
{
    return DbScaleTick{
            .position = math::linear_map(dB, -60.f, 0.f, 0.05f, 1.f),
            .dB = dB};
}

static constexpr auto
makeVolumeFaderScaleTick60To20(float const dB) noexcept -> DbScaleTick
{
    return DbScaleTick{
            .position = math::linear_map(dB, -60.f, -20.f, 0.05f, 0.35f),
            .dB = dB};
}

static constexpr auto
makeVolumeFaderScaleTick16() noexcept -> DbScaleTick
{
    return DbScaleTick{
            .position = math::linear_map(-16.f, -20.f, -12.f, 0.35f, 0.45f),
            .dB = -16.f};
}

static constexpr auto
makeVolumeFaderScaleTick12to12(float const dB) noexcept -> DbScaleTick
{
    return DbScaleTick{
            .position = math::linear_map(dB, -12.f, 12.f, 0.45f, 1.f),
            .dB = dB};
}

static constexpr auto
makeSendFaderScaleTick60To24(float const dB) noexcept -> DbScaleTick
{
    return DbScaleTick{
            .position = math::linear_map(dB, -60.f, -24.f, 0.05f, 0.4f),
            .dB = dB};
}

static constexpr auto
makeVolumeFaderScaleTick24to0(float const dB) noexcept -> DbScaleTick
{
    return DbScaleTick{
            .position = math::linear_map(dB, -24.f, 0.f, 0.4f, 1.f),
            .dB = dB};
}

MixerDbScales::MixerDbScales()
    : m_levelMeterScale(std::make_unique<DbScaleData>(QVector<DbScaleTick>(
              {DbScaleTick{
                       .position = 0.f,
                       .dB = -std::numeric_limits<float>::infinity()},
               DbScaleTick{.position = 0.05f, .dB = -60.f},
               makeLevelMeterScaleTick(-54.f),
               makeLevelMeterScaleTick(-48.f),
               makeLevelMeterScaleTick(-42.f),
               makeLevelMeterScaleTick(-36.f),
               makeLevelMeterScaleTick(-30.f),
               makeLevelMeterScaleTick(-24.f),
               makeLevelMeterScaleTick(-18.f),
               makeLevelMeterScaleTick(-12.f),
               makeLevelMeterScaleTick(-6.f),
               DbScaleTick{.position = 1.f, .dB = 0.f}})))
    , m_volumeFaderScale(std::make_unique<DbScaleData>(QVector<DbScaleTick>(
              {DbScaleTick{
                       .position = 0.f,
                       .dB = -std::numeric_limits<float>::infinity()},
               DbScaleTick{.position = 0.05f, .dB = -60.f},
               makeVolumeFaderScaleTick60To20(-50.f),
               makeVolumeFaderScaleTick60To20(-40.f),
               makeVolumeFaderScaleTick60To20(-30.f),
               DbScaleTick{.position = 0.35f, .dB = -20.f},
               makeVolumeFaderScaleTick16(),
               DbScaleTick{.position = 0.45f, .dB = -12.f},
               makeVolumeFaderScaleTick12to12(-9.f),
               makeVolumeFaderScaleTick12to12(-6.f),
               makeVolumeFaderScaleTick12to12(-3.f),
               makeVolumeFaderScaleTick12to12(0.f),
               makeVolumeFaderScaleTick12to12(3.f),
               makeVolumeFaderScaleTick12to12(6.f),
               makeVolumeFaderScaleTick12to12(9.f),
               DbScaleTick{.position = 1.f, .dB = 12.f}})))
    , m_sendFaderScale(std::make_unique<DbScaleData>(QVector<DbScaleTick>(
              {DbScaleTick{
                       .position = 0.f,
                       .dB = -std::numeric_limits<float>::infinity()},
               DbScaleTick{.position = 0.05f, .dB = -60.f},
               makeSendFaderScaleTick60To24(-50.f),
               makeSendFaderScaleTick60To24(-40.f),
               makeSendFaderScaleTick60To24(-30.f),
               DbScaleTick{.position = 0.4f, .dB = -24.f},
               makeVolumeFaderScaleTick24to0(-21.f),
               makeVolumeFaderScaleTick24to0(-18.f),
               makeVolumeFaderScaleTick24to0(-15.f),
               makeVolumeFaderScaleTick24to0(-12.f),
               makeVolumeFaderScaleTick24to0(-9.f),
               makeVolumeFaderScaleTick24to0(-6.f),
               makeVolumeFaderScaleTick24to0(-3.f),
               DbScaleTick{.position = 1.f, .dB = 0.f}})))
{
}

MixerDbScales::~MixerDbScales() = default;

MixerDbScales g_mixerDbScales;

} // namespace piejam::gui::model
