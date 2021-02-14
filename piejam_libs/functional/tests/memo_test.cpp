// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/functional/memo.h>

#include <gtest/gtest.h>

namespace piejam::test
{

TEST(memo, is_called_only_once_when_called_with_same_arguments_twice)
{
    int called{};
    memo m([&called](int x, int y) {
        ++called;
        return x + y;
    });

    EXPECT_EQ(5, m(2, 3));
    EXPECT_EQ(1, called);

    EXPECT_EQ(5, m(2, 3));
    EXPECT_EQ(1, called);
}

TEST(memo, is_recalled_when_called_with_new_arguments)
{
    int called{};
    memo m([&called](int x, int y) {
        ++called;
        return x + y;
    });

    EXPECT_EQ(5, m(2, 3));
    EXPECT_EQ(1, called);

    EXPECT_EQ(5, m(3, 2));
    EXPECT_EQ(2, called);

    static_assert(std::is_same_v<int const&, decltype(m(2, 3))>);
}

} // namespace piejam::test
