// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/stride_pointer_iterator.h>

#include <gtest/gtest.h>

#include <array>
#include <forward_list>
#include <iterator>
#include <list>

namespace piejam::range::test
{

TEST(stride_pointer_iterator, static_asserts)
{
    static_assert(
            std::random_access_iterator<stride_pointer_iterator<int const>>);
    static_assert(std::random_access_iterator<stride_pointer_iterator<int>>);
    static_assert(std::contiguous_iterator<stride_pointer_iterator<int, 1>>);
    static_assert(
            std::contiguous_iterator<stride_pointer_iterator<int const, 1>>);
}

TEST(stride_pointer_iterator, pre_increment)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(arr.data(), 2);

        EXPECT_EQ(1, *it);
        ++it;
        EXPECT_EQ(3, *it);
        ++it;
        EXPECT_EQ(5, *it);
        ++it;
        EXPECT_EQ(7, *it);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                arr.data());

        EXPECT_EQ(1, *it);
        ++it;
        EXPECT_EQ(3, *it);
        ++it;
        EXPECT_EQ(5, *it);
        ++it;
        EXPECT_EQ(7, *it);
    }
}

TEST(stride_pointer_iterator, post_increment)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(arr.data(), 2);

        EXPECT_EQ(1, *it++);
        EXPECT_EQ(3, *it);
        EXPECT_EQ(3, *it++);
        EXPECT_EQ(5, *it);
        EXPECT_EQ(5, *it++);
        EXPECT_EQ(7, *it);
        EXPECT_EQ(7, *it++);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                arr.data());

        EXPECT_EQ(1, *it++);
        EXPECT_EQ(3, *it);
        EXPECT_EQ(3, *it++);
        EXPECT_EQ(5, *it);
        EXPECT_EQ(5, *it++);
        EXPECT_EQ(7, *it);
        EXPECT_EQ(7, *it++);
    }
}

TEST(stride_pointer_iterator, add_assign)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(arr.data(), 2);

        it += 2;

        EXPECT_EQ(5, *it);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                arr.data());

        it += 2;

        EXPECT_EQ(5, *it);
    }
}

TEST(stride_pointer_iterator, pre_decrement)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(std::next(arr.data(), 6), 2);

        EXPECT_EQ(7, *it);
        --it;
        EXPECT_EQ(5, *it);
        --it;
        EXPECT_EQ(3, *it);
        --it;
        EXPECT_EQ(1, *it);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                std::next(arr.data(), 6));

        EXPECT_EQ(7, *it);
        --it;
        EXPECT_EQ(5, *it);
        --it;
        EXPECT_EQ(3, *it);
        --it;
        EXPECT_EQ(1, *it);
    }
}

TEST(stride_pointer_iterator, post_decrement)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(std::next(arr.data(), 6), 2);

        EXPECT_EQ(7, *it--);
        EXPECT_EQ(5, *it);
        EXPECT_EQ(5, *it--);
        EXPECT_EQ(3, *it);
        EXPECT_EQ(3, *it--);
        EXPECT_EQ(1, *it);
        EXPECT_EQ(1, *it--);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                std::next(arr.data(), 6));

        EXPECT_EQ(7, *it--);
        EXPECT_EQ(5, *it);
        EXPECT_EQ(5, *it--);
        EXPECT_EQ(3, *it);
        EXPECT_EQ(3, *it--);
        EXPECT_EQ(1, *it);
        EXPECT_EQ(1, *it--);
    }
}

TEST(stride_pointer_iterator, subtract_assign)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(std::next(arr.data(), 6), 2);

        it -= 2;

        EXPECT_EQ(3, *it);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                std::next(arr.data(), 6));

        it -= 2;

        EXPECT_EQ(3, *it);
    }
}

TEST(stride_pointer_iterator, equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it1(arr.data(), 2);
        stride_pointer_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_TRUE(it1 == it1);
        EXPECT_FALSE(it1 == it2);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_TRUE(it1 == it1);
        EXPECT_FALSE(it1 == it2);
    }
}

TEST(stride_pointer_iterator, not_equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it1(arr.data(), 2);
        stride_pointer_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_FALSE(it1 != it1);
        EXPECT_TRUE(it1 != it2);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_FALSE(it1 != it1);
        EXPECT_TRUE(it1 != it2);
    }
}

TEST(stride_pointer_iterator, less)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it1(arr.data(), 2);
        stride_pointer_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_FALSE(it1 < it1);
        EXPECT_FALSE(it2 < it1);
        EXPECT_TRUE(it1 < it2);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_FALSE(it1 < it1);
        EXPECT_FALSE(it2 < it1);
        EXPECT_TRUE(it1 < it2);
    }
}

TEST(stride_pointer_iterator, less_equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it1(arr.data(), 2);
        stride_pointer_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_TRUE(it1 <= it1);
        EXPECT_FALSE(it2 <= it1);
        EXPECT_TRUE(it1 <= it2);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_TRUE(it1 <= it1);
        EXPECT_FALSE(it2 <= it1);
        EXPECT_TRUE(it1 <= it2);
    }
}

TEST(stride_pointer_iterator, greater)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it1(arr.data(), 2);
        stride_pointer_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_FALSE(it1 > it1);
        EXPECT_TRUE(it2 > it1);
        EXPECT_FALSE(it1 > it2);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_FALSE(it1 > it1);
        EXPECT_TRUE(it2 > it1);
        EXPECT_FALSE(it1 > it2);
    }
}

TEST(stride_pointer_iterator, greater_equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it1(arr.data(), 2);
        stride_pointer_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_TRUE(it1 >= it1);
        EXPECT_TRUE(it2 >= it1);
        EXPECT_FALSE(it1 >= it2);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_TRUE(it1 >= it1);
        EXPECT_TRUE(it2 >= it1);
        EXPECT_FALSE(it1 >= it2);
    }
}

TEST(stride_pointer_iterator, add_step)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(arr.data(), 2);

        auto it1 = it + 2;
        auto it2 = 2 + it;

        EXPECT_EQ(1, *it);
        EXPECT_EQ(5, *it1);
        EXPECT_EQ(5, *it2);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                arr.data());

        auto it1 = it + 2;
        auto it2 = 2 + it;

        EXPECT_EQ(1, *it);
        EXPECT_EQ(5, *it1);
        EXPECT_EQ(5, *it2);
    }
}

TEST(stride_pointer_iterator, subtract_step)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it(std::next(arr.data(), 6), 2);

        auto it1 = it - 2;

        EXPECT_EQ(7, *it);
        EXPECT_EQ(3, *it1);
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it(
                std::next(arr.data(), 6));

        auto it1 = it - 2;

        EXPECT_EQ(7, *it);
        EXPECT_EQ(3, *it1);
    }
}

TEST(stride_pointer_iterator, diff)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        stride_pointer_iterator it1(arr.data(), 2);
        stride_pointer_iterator it2(std::next(arr.data(), 4), 2);

        EXPECT_EQ(2, it2 - it1);
        EXPECT_EQ(-2, it1 - it2);
        EXPECT_EQ(2, std::distance(it1, it2));
    }

    {
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        stride_pointer_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 4));

        EXPECT_EQ(2, it2 - it1);
        EXPECT_EQ(-2, it1 - it2);
        EXPECT_EQ(2, std::distance(it1, it2));
    }
}

} // namespace piejam::range::test
