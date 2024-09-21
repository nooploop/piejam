// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/numeric/bitops.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

namespace piejam::numeric::bitops::test
{

template <typename T>
struct bitops_toggle : ::testing::Test
{
    using type = T;
};

template <typename T>
struct bitops_toggle_0_6 : bitops_toggle<T>
{
};

using toggle_0_6_types = ::testing::Types<
        std::int8_t,
        std::int16_t,
        std::int32_t,
        std::int64_t,
        std::uint8_t,
        std::uint16_t,
        std::uint32_t,
        std::uint64_t>;
TYPED_TEST_CASE(bitops_toggle_0_6, toggle_0_6_types);

TYPED_TEST(bitops_toggle_0_6, enable)
{
    using type = typename TestFixture::type;
    for (int i = 0; i <= 6; ++i)
    {
        EXPECT_EQ(type{1} << i, toggle(type{}, i));
    }
}

TEST(bitops_toggle, enable_int8_7)
{
    using type = std::int8_t;
    EXPECT_EQ(type{-128}, toggle(type{}, 7));
}

TEST(bitops_toggle, enable_uint8_7)
{
    using type = std::uint8_t;
    EXPECT_EQ(type{128}, toggle(type{}, 7));
}

template <typename T>
struct bitops_toggle_7_14 : bitops_toggle<T>
{
};

using toggle_7_14_types = ::testing::Types<
        std::int16_t,
        std::int32_t,
        std::int64_t,
        std::uint16_t,
        std::uint32_t,
        std::uint64_t>;
TYPED_TEST_CASE(bitops_toggle_7_14, toggle_7_14_types);

TYPED_TEST(bitops_toggle_7_14, enable)
{
    using type = typename TestFixture::type;
    for (int i = 7; i <= 14; ++i)
    {
        EXPECT_EQ(type{1} << i, toggle(type{}, i));
    }
}

TEST(bitops_toggle, enable_int16_15)
{
    using type = std::int16_t;
    EXPECT_EQ(type{-32768}, toggle(type{}, 15));
}

TEST(bitops_toggle, enable_uint16_15)
{
    using type = std::uint16_t;
    EXPECT_EQ(type{32768}, toggle(type{}, 15));
}

} // namespace piejam::numeric::bitops::test
