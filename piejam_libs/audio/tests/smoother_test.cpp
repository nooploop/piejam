// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    auto it = sut.input_iterator();
    ++it;

    EXPECT_FLOAT_EQ(0.25f, *it);
    EXPECT_FLOAT_EQ(0.25f, *sut);
}

TEST(smoother, post_increment)
{
    smoother<float> sut;
    sut.set(1.f, 4);

    auto it = sut.input_iterator();
    EXPECT_FLOAT_EQ(0.f, *it);
    EXPECT_FLOAT_EQ(0.25f, *it++);
    EXPECT_FLOAT_EQ(0.25f, *sut);
}

TEST(smoother, increment_to_target)
{
    smoother<float> sut;
    sut.set(1.f, 3);

    auto it = sut.input_iterator();
    ++it;
    ++it;
    ++it;

    EXPECT_EQ(1.f, *it);
    EXPECT_EQ(1.f, *sut);
}

TEST(smoother, increment_past_target)
{
    smoother<float> sut;
    sut.set(1.f, 3);

    auto it = sut.input_iterator();
    ++it;
    ++it;
    ++it;
    ++it;

    EXPECT_EQ(1.f, *it);
    EXPECT_EQ(1.f, *sut);
}

TEST(smoother, iterator_category_is_input_iterator)
{
    static_assert(std::is_same_v<
                  std::input_iterator_tag,
                  std::iterator_traits<
                          smoother<float>::iterator>::iterator_category>);
}

} // namespace piejam::audio::test
