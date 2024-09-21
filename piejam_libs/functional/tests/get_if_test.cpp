// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/functional/get_if.h>

#include <gtest/gtest.h>

#include <string>

namespace piejam::test
{

TEST(get_if_by_type, fitting)
{
    std::variant<int, float, std::string> vi{5};

    auto const* const i = get_if_by_type<int>(&vi);

    ASSERT_NE(i, nullptr);
    EXPECT_EQ(*i, 5);
}

TEST(get_if_by_type, non_fitting)
{
    std::variant<int, float, std::string> vi{5};

    auto const* const s = get_if_by_type<std::string>(&vi);

    EXPECT_EQ(s, nullptr);
}

TEST(get_if_by_index, fitting)
{
    std::variant<int, float, std::string> vi{5};

    auto const* const i = get_if_by_index<0>(&vi);

    ASSERT_NE(i, nullptr);
    EXPECT_EQ(*i, 5);
}

TEST(get_if_by_index, non_fitting)
{
    std::variant<int, float, std::string> vi{5};

    auto const* const s = get_if_by_index<2>(&vi);

    EXPECT_EQ(s, nullptr);
}

} // namespace piejam::test
