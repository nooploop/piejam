// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/transform_to_vector.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <array>

namespace piejam::algorithm::test
{

TEST(transform_to_vector, from_empty_range_will_result_in_empty_vector)
{
    auto vec = transform_to_vector(std::vector<int>{}, [](int x) {
        return x * 2;
    });

    EXPECT_TRUE(vec.empty());
}

TEST(transform_to_vector, transform)
{
    auto vec = transform_to_vector(std::array{2, 3, 5}, [](int x) {
        return x * 2;
    });

    EXPECT_TRUE(testing::Matches(testing::ElementsAre(4, 6, 10))(vec));
}

} // namespace piejam::algorithm::test
