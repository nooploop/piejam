// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/shift_push_back.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <array>

namespace piejam::algorithm::test
{

TEST(shift_push_back, source_is_smaller_than_target)
{
    std::array target{2, 2, 2, 2, 2};
    std::array source{3, 4};

    shift_push_back(target, source);

    EXPECT_TRUE(testing::Matches(testing::ElementsAre(2, 2, 2, 3, 4))(target));
}

TEST(shift_push_back, source_is_bigger_than_target)
{
    std::array target{2, 2, 2};
    std::array source{1, 2, 3, 4, 5};

    shift_push_back(target, source);

    EXPECT_TRUE(testing::Matches(testing::ElementsAre(3, 4, 5))(target));
}

} // namespace piejam::algorithm::test
