// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/pan.h>

#include <gtest/gtest.h>

#include <numbers>

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

    constexpr float one_div_root_two = 1.f / std::numbers::sqrt2_v<float>;

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

TEST(sinusoidal_constant_power_pan, compare_to_exact)
{
    for (float x = -1.f; x <= 1.f; x += .1f)
    {
        auto exact = sinusoidal_constant_power_pan_exact(x);
        auto sut = sinusoidal_constant_power_pan(x);
        EXPECT_NEAR(exact.left, sut.left, 0.002f);
        EXPECT_NEAR(exact.right, sut.right, 0.002f);
    }
}

TEST(stereo_balance, zero)
{
    auto sut = stereo_balance(0.f);

    EXPECT_FLOAT_EQ(1.f, sut.left);
    EXPECT_FLOAT_EQ(1.f, sut.right);
}

TEST(stereo_balance, minus_one)
{
    auto sut = stereo_balance(-1.f);

    EXPECT_FLOAT_EQ(1.f, sut.left);
    EXPECT_FLOAT_EQ(0.f, sut.right);
}

TEST(stereo_balance, plus_one)
{
    auto sut = stereo_balance(1.f);

    EXPECT_FLOAT_EQ(0.f, sut.left);
    EXPECT_FLOAT_EQ(1.f, sut.right);
}

TEST(stereo_balance, move_to_left)
{
    for (float x = 0.f; x >= -1.f; x += -0.1f)
    {
        auto sut = stereo_balance(x);

        EXPECT_FLOAT_EQ(1.f, sut.left);
    }
}

TEST(stereo_balance, move_to_right)
{
    for (float x = 0.f; x <= 1.f; x += 0.1f)
    {
        auto sut = stereo_balance(x);

        EXPECT_FLOAT_EQ(1.f, sut.right);
    }
}

} // namespace piejam::audio::test
