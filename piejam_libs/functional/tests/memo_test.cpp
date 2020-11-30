// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

    EXPECT_EQ(7, m(4, 3));
    EXPECT_EQ(2, called);

    static_assert(std::is_same_v<int const&, decltype(m(2, 3))>);
}

} // namespace piejam::test
