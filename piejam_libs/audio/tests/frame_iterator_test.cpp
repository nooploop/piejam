// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/frame_iterator.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(frame_iterator, static_assert_test)
{
    static_assert(std::random_access_iterator<frame_iterator<float, 0>>);
    static_assert(std::random_access_iterator<frame_iterator<float, 1>>);
    static_assert(std::random_access_iterator<frame_iterator<float, 2>>);
}

TEST(frame_iterator, non_const_to_const)
{
    std::array<float, 2> a{1.f, -1.f};
    frame_iterator<float, 2> nc_it(a);
    frame_iterator<float const, 2> it = nc_it;

    EXPECT_EQ(1.f, (*it)[0]);
    EXPECT_EQ(-1.f, (*it)[1]);
}

TEST(frame_iterator, static_to_runtime)
{
    std::array<float, 2> a{1.f, -1.f};
    frame_iterator<float, 2> nc_it(a);
    frame_iterator<float, 0> it = nc_it;

    EXPECT_EQ(1.f, (*it)[0]);
    EXPECT_EQ(-1.f, (*it)[1]);
}

TEST(frame_iterator, static_non_const_to_runtime_const)
{
    std::array<float, 2> a{1.f, -1.f};
    frame_iterator<float, 2> nc_it(a);
    frame_iterator<float const, 0> it = nc_it;

    EXPECT_EQ(1.f, (*it)[0]);
    EXPECT_EQ(-1.f, (*it)[1]);
}

} // namespace piejam::audio::test
