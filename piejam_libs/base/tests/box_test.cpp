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

#include <piejam/box.h>

#include <gtest/gtest.h>

namespace piejam::container::test
{

TEST(box, default_ctor_and_get)
{
    box<int> x;
    EXPECT_EQ(int{}, x.get());
}

TEST(box, ctor_with_args_and_get)
{
    box<int> x(5);
    EXPECT_EQ(int{5}, x.get());
}

TEST(box, copy_ctor)
{
    box<int> x(5);
    box<int> y(x);
    EXPECT_EQ(5, y.get());
}

TEST(box, assign_operator)
{
    box<int> x(5);
    box<int> y(6);
    ASSERT_EQ(5, x.get());

    x = y;

    EXPECT_EQ(6, x.get());
}

TEST(box, cast_operator)
{
    box<int> x(5);
    int const& y = x;
    EXPECT_EQ(5, y);
}

TEST(box, deref_operator)
{
    box<int> x(5);
    EXPECT_EQ(5, *x);
}

TEST(box, arrow_operator)
{
    struct struct_with_method
    {
        int meth() const noexcept { return 5; }
    };

    box<struct_with_method> x;
    EXPECT_EQ(5, x->meth());
}

TEST(box, eq_operator)
{
    box<int> x(5);
    box<int> y(5);
    box<int> z(7);

    EXPECT_TRUE(x == x);
    EXPECT_TRUE(x == y);
    EXPECT_FALSE(x == z);
}

TEST(box, neq_operator)
{
    box<int> x(5);
    box<int> y(5);
    box<int> z(7);

    EXPECT_FALSE(x != x);
    EXPECT_FALSE(x != y);
    EXPECT_TRUE(x != z);
}

TEST(box, assign_lvalue)
{
    box<int> x(5);
    box<int> y(x);

    int const v = 6;

    x = v;

    EXPECT_EQ(6, x.get());
    EXPECT_EQ(5, y.get());
}

TEST(box, assign_rvalue)
{
    box<int> x(5);
    box<int> y(x);

    auto f = []() -> int { return 6; };

    x = f();

    EXPECT_EQ(6, x.get());
    EXPECT_EQ(5, y.get());
}

TEST(box, update_without_return_value)
{
    box<int> sut(5);

    sut.update([](int& x) { x = 7; });

    EXPECT_EQ(7, sut.get());
}

TEST(box, update_with_return_value)
{
    box<int> sut(5);

    auto res = sut.update([](int& x) {
        auto y = x;
        x = 7;
        return y;
    });

    EXPECT_EQ(7, sut.get());
    EXPECT_EQ(5, res);
}

} // namespace piejam::container::test
