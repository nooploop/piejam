// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/for_each_adjacent.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <array>
#include <string>
#include <vector>

namespace piejam::algorithm::test
{

TEST(for_each_adjacent, on_empty_range)
{
    std::vector<int> result;
    for_each_adjacent(std::vector<int>(), [&result](auto x, auto y) {
        result.push_back(x + y);
    });
    EXPECT_TRUE(result.empty());
}

TEST(for_each_adjacent, on_range_with_one_element)
{
    std::vector<int> result;
    for_each_adjacent(std::array{2}, [&result](auto x, auto y) {
        result.push_back(x + y);
    });
    EXPECT_TRUE(result.empty());
}

TEST(for_each_adjacent, on_range_with_two_elements)
{
    std::vector<int> result;
    for_each_adjacent(std::array{5, 7}, [&result](auto x, auto y) {
        result.push_back(x + y);
    });

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(12))(result));
}

TEST(for_each_adjacent, on_range_with_multiple_elements)
{
    std::vector<int> result;
    for_each_adjacent(
            std::array{2, 3, 5, 7, 11, 13},
            [&result](auto x, auto y) { result.push_back(x + y); });

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(5, 8, 12, 18, 24))(result));
}

TEST(for_each_adjacent, binary_op_can_take_non_const_reference)
{
    using namespace std::string_literals;
    std::vector vec{"aa"s, "bb"s, "cc"s, "dd"s};
    for_each_adjacent(vec, [](auto& x, auto& y) {
        static_assert(!std::is_const_v<decltype(x)>);
        static_assert(!std::is_const_v<decltype(y)>);
        x += '0';
        y += '0';
    });

    EXPECT_THAT(vec, testing::ElementsAre("aa0"s, "bb00"s, "cc00"s, "dd0"s));
}

} // namespace piejam::algorithm::test
