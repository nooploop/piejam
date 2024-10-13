// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/pointer_stride_iterator.h>

#include <gtest/gtest.h>

#include <array>
#include <iterator>

namespace piejam::range::test
{

TEST(pointer_stride_iterator, static_asserts)
{
    static_assert(
            std::random_access_iterator<pointer_stride_iterator<int const>>);
    static_assert(std::random_access_iterator<pointer_stride_iterator<int>>);
    static_assert(
            !std::contiguous_iterator<pointer_stride_iterator<int const>>);
    static_assert(!std::contiguous_iterator<pointer_stride_iterator<int>>);

    static_assert(
            std::random_access_iterator<pointer_stride_iterator<int const, 2>>);
    static_assert(std::random_access_iterator<pointer_stride_iterator<int, 2>>);
    static_assert(
            !std::contiguous_iterator<pointer_stride_iterator<int const, 2>>);
    static_assert(!std::contiguous_iterator<pointer_stride_iterator<int, 2>>);

    static_assert(std::contiguous_iterator<pointer_stride_iterator<int, 1>>);
    static_assert(
            std::contiguous_iterator<pointer_stride_iterator<int const, 1>>);
}

TEST(pointer_stride_iterator, pre_increment)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(arr.data(), 2);

        EXPECT_EQ(1, *it);
        ++it;
        EXPECT_EQ(3, *it);
        ++it;
        EXPECT_EQ(5, *it);
        ++it;
        EXPECT_EQ(7, *it);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
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

TEST(pointer_stride_iterator, post_increment)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(arr.data(), 2);

        EXPECT_EQ(1, *it++);
        EXPECT_EQ(3, *it);
        EXPECT_EQ(3, *it++);
        EXPECT_EQ(5, *it);
        EXPECT_EQ(5, *it++);
        EXPECT_EQ(7, *it);
        EXPECT_EQ(7, *it++);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
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

TEST(pointer_stride_iterator, add_assign)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(arr.data(), 2);

        it += 2;

        EXPECT_EQ(5, *it);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
                arr.data());

        it += 2;

        EXPECT_EQ(5, *it);
    }
}

TEST(pointer_stride_iterator, pre_decrement)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(std::next(arr.data(), 6), 2);

        EXPECT_EQ(7, *it);
        --it;
        EXPECT_EQ(5, *it);
        --it;
        EXPECT_EQ(3, *it);
        --it;
        EXPECT_EQ(1, *it);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
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

TEST(pointer_stride_iterator, post_decrement)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(std::next(arr.data(), 6), 2);

        EXPECT_EQ(7, *it--);
        EXPECT_EQ(5, *it);
        EXPECT_EQ(5, *it--);
        EXPECT_EQ(3, *it);
        EXPECT_EQ(3, *it--);
        EXPECT_EQ(1, *it);
        EXPECT_EQ(1, *it--);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
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

TEST(pointer_stride_iterator, subtract_assign)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(std::next(arr.data(), 6), 2);

        it -= 2;

        EXPECT_EQ(3, *it);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
                std::next(arr.data(), 6));

        it -= 2;

        EXPECT_EQ(3, *it);
    }
}

TEST(pointer_stride_iterator, equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it1(arr.data(), 2);
        pointer_stride_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_TRUE(it1 == it1);
        EXPECT_FALSE(it1 == it2);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_TRUE(it1 == it1);
        EXPECT_FALSE(it1 == it2);
    }
}

TEST(pointer_stride_iterator, not_equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it1(arr.data(), 2);
        pointer_stride_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_FALSE(it1 != it1);
        EXPECT_TRUE(it1 != it2);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_FALSE(it1 != it1);
        EXPECT_TRUE(it1 != it2);
    }
}

TEST(pointer_stride_iterator, less)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it1(arr.data(), 2);
        pointer_stride_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_FALSE(it1 < it1);
        EXPECT_FALSE(it2 < it1);
        EXPECT_TRUE(it1 < it2);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_FALSE(it1 < it1);
        EXPECT_FALSE(it2 < it1);
        EXPECT_TRUE(it1 < it2);
    }
}

TEST(pointer_stride_iterator, less_equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it1(arr.data(), 2);
        pointer_stride_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_TRUE(it1 <= it1);
        EXPECT_FALSE(it2 <= it1);
        EXPECT_TRUE(it1 <= it2);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_TRUE(it1 <= it1);
        EXPECT_FALSE(it2 <= it1);
        EXPECT_TRUE(it1 <= it2);
    }
}

TEST(pointer_stride_iterator, greater)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it1(arr.data(), 2);
        pointer_stride_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_FALSE(it1 > it1);
        EXPECT_TRUE(it2 > it1);
        EXPECT_FALSE(it1 > it2);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_FALSE(it1 > it1);
        EXPECT_TRUE(it2 > it1);
        EXPECT_FALSE(it1 > it2);
    }
}

TEST(pointer_stride_iterator, greater_equal)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it1(arr.data(), 2);
        pointer_stride_iterator it2(std::next(arr.data(), 2), 2);

        EXPECT_TRUE(it1 >= it1);
        EXPECT_TRUE(it2 >= it1);
        EXPECT_FALSE(it1 >= it2);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 2));

        EXPECT_TRUE(it1 >= it1);
        EXPECT_TRUE(it2 >= it1);
        EXPECT_FALSE(it1 >= it2);
    }
}

TEST(pointer_stride_iterator, add_step)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(arr.data(), 2);

        auto it1 = it + 2;
        auto it2 = 2 + it;

        EXPECT_EQ(1, *it);
        EXPECT_EQ(5, *it1);
        EXPECT_EQ(5, *it2);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
                arr.data());

        auto it1 = it + 2;
        auto it2 = 2 + it;

        EXPECT_EQ(1, *it);
        EXPECT_EQ(5, *it1);
        EXPECT_EQ(5, *it2);
    }
}

TEST(pointer_stride_iterator, subtract_step)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it(std::next(arr.data(), 6), 2);

        auto it1 = it - 2;

        EXPECT_EQ(7, *it);
        EXPECT_EQ(3, *it1);
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it(
                std::next(arr.data(), 6));

        auto it1 = it - 2;

        EXPECT_EQ(7, *it);
        EXPECT_EQ(3, *it1);
    }
}

TEST(pointer_stride_iterator, diff)
{
    std::array arr{1, 2, 3, 4, 5, 6, 7};

    {
        pointer_stride_iterator it1(arr.data(), 2);
        pointer_stride_iterator it2(std::next(arr.data(), 4), 2);

        EXPECT_EQ(2, it2 - it1);
        EXPECT_EQ(-2, it1 - it2);
        EXPECT_EQ(2, std::distance(it1, it2));
    }

    {
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it1(
                arr.data());
        pointer_stride_iterator<decltype(arr)::value_type const, 2> it2(
                std::next(arr.data(), 4));

        EXPECT_EQ(2, it2 - it1);
        EXPECT_EQ(-2, it1 - it2);
        EXPECT_EQ(2, std::distance(it1, it2));
    }
}

} // namespace piejam::range::test
