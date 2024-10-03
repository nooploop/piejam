// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/find_or_get_first.h>

#include <gtest/gtest.h>

#include <array>

namespace piejam::algorithm::test
{

static constexpr std::array s_arr{2, 3, 5, 7, 11, 13};

TEST(find_or_get_first, returns_iterator_to_element_if_found)
{
    auto it = find_or_get_first(s_arr, 11);
    ASSERT_NE(it, s_arr.end());
    EXPECT_EQ(*it, 11);
}

TEST(find_or_get_first, returns_iterator_to_first_if_not_found)
{
    auto it = find_or_get_first(s_arr, 12);
    ASSERT_NE(it, s_arr.end());
    EXPECT_EQ(*it, 2);
}

TEST(find_or_get_first, returns_end_if_empty_range_is_passed)
{
    std::array<int, 0> arr{};
    auto it = find_or_get_first(arr, 5);
    EXPECT_EQ(it, arr.end());
}

} // namespace piejam::algorithm::test
