// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/audio/smoother.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(smoother, default_ctor_current_is_zero)
{
    EXPECT_EQ(0.f, *smoother<float>());
}

TEST(smoother, set_with_zero_smooth_frame_time_set_current_instantly)
{
    smoother<float> sut;
    sut.set(1.f, 0);
    EXPECT_EQ(1.f, *sut);
}

TEST(smoother, set_with_non_zero_smooth_frame_time_will_not_change_current)
{
    smoother<float> sut;
    sut.set(1.f, 4);
    EXPECT_EQ(0.f, *sut);
}

TEST(smoother, pre_increment)
{
    smoother<float> sut;
    sut.set(1.f, 4);

    auto res = ++sut;

    EXPECT_FLOAT_EQ(0.25f, *res);
    EXPECT_FLOAT_EQ(0.25f, *sut);
}

TEST(smoother, post_increment)
{
    smoother<float> sut;
    sut.set(1.f, 4);

    auto res = sut++;

    EXPECT_FLOAT_EQ(0.f, *res);
    EXPECT_FLOAT_EQ(0.25f, *sut);
}

TEST(smoother, increment_to_target)
{
    smoother<float> sut;
    sut.set(1.f, 3);

    ++sut;
    ++sut;
    ++sut;

    EXPECT_EQ(1.f, *sut);
}

TEST(smoother, increment_past_target)
{
    smoother<float> sut;
    sut.set(1.f, 3);

    ++sut;
    ++sut;
    ++sut;
    ++sut;

    EXPECT_EQ(1.f, *sut);
}

} // namespace piejam::audio::test
