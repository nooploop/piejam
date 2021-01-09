// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/pan.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(sinusoidal_constant_power_pan, minus_one)
{
    auto sut = sinusoidal_constant_power_pan(-1.f);

    EXPECT_NEAR(1.f, sut.left, 0.002f);
    EXPECT_LE(sut.left, 1.f);

    EXPECT_NEAR(0.f, sut.right, 0.002f);
    EXPECT_GE(sut.right, 0.f);
}

TEST(sinusoidal_constant_power_pan, zero)
{
    auto sut = sinusoidal_constant_power_pan(0.f);

    using namespace boost::math::float_constants;

    EXPECT_FLOAT_EQ(sut.left, sut.right);
    EXPECT_FLOAT_EQ(one_div_root_two, sut.left);
    EXPECT_FLOAT_EQ(one_div_root_two, sut.right);
}

TEST(sinusoidal_constant_power_pan, plus_one)
{
    auto sut = sinusoidal_constant_power_pan(1.f);

    EXPECT_NEAR(0.f, sut.left, 0.002f);
    EXPECT_GE(sut.left, 0.f);

    EXPECT_NEAR(1.f, sut.right, 0.002f);
    EXPECT_LE(sut.right, 1.f);
}

} // namespace piejam::audio::test
