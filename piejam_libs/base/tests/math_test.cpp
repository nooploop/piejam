// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/math.h>

#include <gtest/gtest.h>

namespace piejam::test
{

TEST(to_dB, default_min)
{
    EXPECT_FLOAT_EQ(math::negative_inf<float>, math::to_dB(0.f));
}

TEST(to_dB, custom_min)
{
    EXPECT_FLOAT_EQ(math::negative_inf<float>, math::to_dB(0.f, 0.1f));
    EXPECT_FLOAT_EQ(math::negative_inf<float>, math::to_dB(0.05f, 0.1f));
    EXPECT_FLOAT_EQ(math::negative_inf<float>, math::to_dB(0.1f, 0.1f));
}

TEST(from_dB, default_min)
{
    EXPECT_FLOAT_EQ(0.f, math::from_dB(math::negative_inf<float>));
}

TEST(from_dB, custom_min)
{
    EXPECT_FLOAT_EQ(0.f, math::from_dB(math::negative_inf<float>, -20.f));
    EXPECT_FLOAT_EQ(0.f, math::from_dB(-60.f, -20.f));
    EXPECT_FLOAT_EQ(0.f, math::from_dB(-20.f, -20.f));
}

} // namespace piejam::test
