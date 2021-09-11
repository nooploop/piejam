// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/DbScaleData.h>

#include <piejam/math.h>

#include <gtest/gtest.h>

namespace piejam::gui::model::test
{

TEST(DbScaleData, verify_position)
{
    static DbScaleData const s_volumeFaderScale{
            {DbScaleTick{
                     .position = 0.f,
                     .dB = -std::numeric_limits<float>::infinity()},
             DbScaleTick{.position = 0.05f, .dB = -60.f},
             DbScaleTick{.position = 0.35f, .dB = -20.f},
             DbScaleTick{.position = 0.45f, .dB = -12.f},
             DbScaleTick{.position = 1.f, .dB = 12.f}}};

    EXPECT_FLOAT_EQ(
            math::linear_map(0.f, -12.f, 12.f, 0.45f, 1.f),
            s_volumeFaderScale.dBToPosition(0.f));
}

} // namespace piejam::gui::model::test
