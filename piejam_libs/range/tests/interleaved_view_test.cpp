// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/interleaved_view.h>

#include <gtest/gtest.h>

#include <array>

namespace piejam::range::test
{

TEST(interleaved_view, concept_check)
{
    static_assert(std::ranges::random_access_range<interleaved_view<float, 2>>);
    static_assert(std::ranges::sized_range<interleaved_view<float, 0>>);
}

TEST(interleaved_view, make_from_data_and_num_frames)
{
    std::array buf{1.f, 2.f, 3.f, 4.f};
    interleaved_view<float, 2> v(buf.data(), 2);

    EXPECT_EQ(v.num_channels(), 2u);
    EXPECT_EQ(v.num_frames(), 2u);

    auto it_begin = v.begin();
    EXPECT_EQ(it_begin->data(), buf.data());

    auto it_end = v.end();
    EXPECT_EQ(it_end->data(), buf.data() + buf.size());
}

TEST(interleaved_view, make_from_data_num_channels_and_num_frames)
{
    std::array buf{1.f, 2.f, 3.f, 4.f};
    interleaved_view<float> v(buf.data(), 2, 2);

    EXPECT_EQ(v.num_channels(), 2u);
    EXPECT_EQ(v.num_frames(), 2u);

    auto it_begin = v.begin();
    EXPECT_EQ(it_begin->data(), buf.data());

    auto it_end = v.end();
    EXPECT_EQ(it_end->data(), buf.data() + buf.size());
}

TEST(interleaved_view, make_from_frame_iterators)
{
    std::array buf{1.f, 2.f, 3.f, 4.f};
    frame_iterator<float> first(std::span(buf.data(), 2));
    frame_iterator<float> last(std::span(buf.data() + buf.size(), 2));

    interleaved_view<float> v(first, last);

    EXPECT_EQ(v.num_channels(), 2u);
    EXPECT_EQ(v.num_frames(), 2u);
}

TEST(interleaved_view, make_from_non_const_to_const)
{
    std::array buf{1.f, 2.f, 3.f, 4.f};
    interleaved_view<float, 2> v(buf.data(), 2);

    interleaved_view<float const, 2> w(v);

    EXPECT_EQ(w.num_channels(), 2u);
    EXPECT_EQ(w.num_frames(), 2u);

    auto it_begin = w.begin();
    EXPECT_EQ(it_begin->data(), buf.data());

    auto it_end = w.end();
    EXPECT_EQ(it_end->data(), buf.data() + buf.size());
}

TEST(interleaved_view, channels_cast)
{
    std::array buf{1.f, 2.f, 3.f, 4.f};
    interleaved_view<float> v(buf.data(), 2, 2);

    interleaved_view<float const, 2> w = v.channels_cast<2>();

    EXPECT_EQ(w.num_channels(), 2u);
    EXPECT_EQ(w.num_frames(), 2u);

    auto it_begin = w.begin();
    EXPECT_EQ(it_begin->data(), buf.data());

    auto it_end = w.end();
    EXPECT_EQ(it_end->data(), buf.data() + buf.size());
}

} // namespace piejam::range::test
