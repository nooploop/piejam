// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/table_view.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

namespace piejam::range::test
{

TEST(table_view, static_asserts)
{
    static_assert(std::random_access_iterator<
                  table_view<float>::major_index_iterator<float>>);
    static_assert(std::ranges::random_access_range<table_view<float>>);
    static_assert(std::ranges::viewable_range<table_view<float> const&>);
}

TEST(table_view, major_index_iterator_equality)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> sut(arr.data(), 2, 3, 4, 1);

    EXPECT_EQ(sut.begin(), sut.begin());
    EXPECT_NE(sut.begin(), sut.end());
    EXPECT_EQ(sut.end(), sut.end());
}

TEST(row_major_table_view, index_access)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> sut(arr.data(), 2, 3, 4, 1);

    ASSERT_EQ(2, sut.major_size());
    EXPECT_THAT(sut[0], testing::ElementsAre(1, 2, 3));
    EXPECT_THAT(sut[1], testing::ElementsAre(4, 5, 6));
}

TEST(row_major_table_view, stepped_columns)
{
    // 1 2 3
    // 4 5 6
    std::vector arr{1, 0, 2, 0, 3, 4, 0, 5, 0, 6};

    table_view<int> sut(arr.data(), 2, 3, 5, 2);

    ASSERT_EQ(2, sut.major_size());
    EXPECT_THAT(sut[0], testing::ElementsAre(1, 2, 3));
    EXPECT_THAT(sut[1], testing::ElementsAre(4, 5, 6));
}

TEST(column_major_table_view, iterate_and_compare)
{
    // 1 2 3
    // 4 5 6
    std::vector arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int> sut(arr.data(), 3, 2, 1, 4);

    ASSERT_EQ(sut.major_size(), 3);
    EXPECT_THAT(sut[0], testing::ElementsAre(1, 4));
    EXPECT_THAT(sut[1], testing::ElementsAre(2, 5));
    EXPECT_THAT(sut[2], testing::ElementsAre(3, 6));
}

TEST(table_view, compile_time_minor_step)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    using tv_t =
            table_view<int, std::dynamic_extent, std::dynamic_extent, 0, 1>;
    tv_t sut(arr.data(), 2, 3, 4, 1);

    ASSERT_EQ(2, sut.major_size());

    static_assert(std::is_same_v<
                  std::span<int>,
                  std::remove_cvref_t<decltype(sut[0])>>);

    static_assert(std::is_same_v<
                  std::span<int>,
                  std::remove_cvref_t<decltype(*sut.begin())>>);

    EXPECT_THAT(sut[0], testing::ElementsAre(1, 2, 3));
    EXPECT_THAT(sut[1], testing::ElementsAre(4, 5, 6));
}

TEST(table_view, compile_time_major_step)
{
    // 1 2 3
    // 4 5 6
    std::array arr{1, 2, 3, 0, 4, 5, 6, 0};

    table_view<int, std::dynamic_extent, std::dynamic_extent, 4, 0>
            sut(arr.data(), 2, 3, 4, 1);

    ASSERT_EQ(2, sut.major_size());
    EXPECT_THAT(sut[0], testing::ElementsAre(1, 2, 3));
    EXPECT_THAT(sut[1], testing::ElementsAre(4, 5, 6));
}

} // namespace piejam::range::test
