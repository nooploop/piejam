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

#include <piejam/numeric/intops.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace piejam::numeric::intops::test
{

template <class T>
struct sign_map_test : ::testing::Test
{
    using type = T;
};

using signed_ints =
        ::testing::Types<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;
TYPED_TEST_CASE(sign_map_test, signed_ints);

TYPED_TEST(sign_map_test, signed_to_unsigned_min)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = std::make_unsigned_t<signed_t>;

    EXPECT_EQ(
            std::numeric_limits<unsigned_t>::min(),
            sign_map<unsigned_t>(std::numeric_limits<signed_t>::min()));
}

TYPED_TEST(sign_map_test, signed_to_unsigned_max)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = std::make_unsigned_t<signed_t>;

    EXPECT_EQ(
            std::numeric_limits<unsigned_t>::max(),
            sign_map<unsigned_t>(std::numeric_limits<signed_t>::max()));
}

TYPED_TEST(sign_map_test, signed_to_unsigned_zero)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = std::make_unsigned_t<signed_t>;

    EXPECT_EQ(
            unsigned_t{1} << (sizeof(unsigned_t) * 8 - 1),
            sign_map<unsigned_t>(signed_t{}));
}

TYPED_TEST(sign_map_test, unsigned_to_signed_min)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = std::make_unsigned_t<signed_t>;

    EXPECT_EQ(
            std::numeric_limits<signed_t>::min(),
            sign_map<signed_t>(std::numeric_limits<unsigned_t>::min()));
}

TYPED_TEST(sign_map_test, unsigned_to_signed_max)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = std::make_unsigned_t<signed_t>;

    EXPECT_EQ(
            std::numeric_limits<signed_t>::max(),
            sign_map<signed_t>(std::numeric_limits<unsigned_t>::max()));
}

TYPED_TEST(sign_map_test, unsigned_to_signed_zero)
{
    using signed_t = typename TestFixture::type;
    using unsigned_t = std::make_unsigned_t<signed_t>;

    EXPECT_EQ(
            signed_t{},
            sign_map<signed_t>(
                    unsigned_t{1ull << (sizeof(unsigned_t) * 8 - 1)}));
}

} // namespace piejam::numeric::intops::test
