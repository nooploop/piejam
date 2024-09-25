// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/numeric/intops.h>

#include <piejam/numeric/limits.h>
#include <piejam/numeric/type_traits.h>

#include <gtest/gtest.h>

#include <cstdint>

namespace piejam::numeric::intops::test
{

template <class T>
struct sign_map_test : ::testing::Test
{
    using type = T;
};

using signed_ints = ::testing::
        Types<std::int8_t, std::int16_t, int24_t, std::int32_t, std::int64_t>;
TYPED_TEST_SUITE(sign_map_test, signed_ints);

TYPED_TEST(sign_map_test, signed_to_unsigned_min)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = make_unsigned_t<signed_t>;

    EXPECT_EQ(
            limits<unsigned_t>::min(),
            sign_map<unsigned_t>(limits<signed_t>::min()));
}

TYPED_TEST(sign_map_test, signed_to_unsigned_max)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = make_unsigned_t<signed_t>;

    EXPECT_EQ(
            limits<unsigned_t>::max(),
            sign_map<unsigned_t>(limits<signed_t>::max()));
}

TYPED_TEST(sign_map_test, signed_to_unsigned_zero)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = make_unsigned_t<signed_t>;

    EXPECT_EQ(
            unsigned_t{1} << sign_bit<unsigned_t>,
            sign_map<unsigned_t>(signed_t{}));
}

TYPED_TEST(sign_map_test, unsigned_to_signed_min)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = make_unsigned_t<signed_t>;

    EXPECT_EQ(
            limits<signed_t>::min(),
            sign_map<signed_t>(limits<unsigned_t>::min()));
}

TYPED_TEST(sign_map_test, unsigned_to_signed_max)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = make_unsigned_t<signed_t>;

    EXPECT_EQ(
            limits<signed_t>::max(),
            sign_map<signed_t>(limits<unsigned_t>::max()));
}

TYPED_TEST(sign_map_test, unsigned_to_signed_zero)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = make_unsigned_t<signed_t>;

    EXPECT_EQ(
            signed_t{},
            sign_map<signed_t>(unsigned_t{1ull << sign_bit<unsigned_t>}));
}

} // namespace piejam::numeric::intops::test
