// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/functional/get.h>

#include <gtest/gtest.h>

#include <string>
#include <tuple>

namespace piejam::test
{

TEST(get, by_index)
{
    std::tuple<int, float, std::string> t{5, 23.f, "foo"};

    EXPECT_EQ(5, get_by_index<0>(t));
    EXPECT_EQ(23.f, get_by_index<1>(t));
    EXPECT_EQ("foo", get_by_index<2>(t));
}

TEST(get, by_type)
{
    std::tuple<int, float, std::string> t{5, 23.f, "foo"};

    EXPECT_EQ(5, get_by_type<int>(t));
    EXPECT_EQ(23.f, get_by_type<float>(t));
    EXPECT_EQ("foo", get_by_type<std::string>(t));
}

} // namespace piejam::test
