// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/numeric/bit.h>
#include <piejam/numeric/intx.h>

#include <gtest/gtest.h>

#include <cstdint>

namespace piejam::numeric::bit::test
{

template <class T>
struct bit_toggle : ::testing::Test
{
    using type = T;
};

using toggle_types = ::testing::Types<
        std::int8_t,
        std::int16_t,
        int24_t,
        std::int32_t,
        std::int64_t,
        std::uint8_t,
        std::uint16_t,
        uint24_t,
        std::uint32_t,
        std::uint64_t>;

TYPED_TEST_SUITE(bit_toggle, toggle_types);

TYPED_TEST(bit_toggle, toggle)
{
    using type = typename TestFixture::type;

    // clang-format off
    [this]<std::size_t... Bs>(std::index_sequence<Bs...>) {(
        [this]<std::size_t B>(std::integral_constant<std::size_t, B>) {
            if constexpr (B < sizeof(type) * CHAR_BIT)
            {
                auto const zero = type{0};
                auto const toggled = type{static_cast<type>(1ul << B)};
                EXPECT_EQ(toggled, toggle(zero, B));
                EXPECT_EQ(zero, toggle(toggled, B));
                EXPECT_EQ(toggled, toggle<B>(zero));
                EXPECT_EQ(zero, toggle<B>(toggled));
            }
        }(std::integral_constant<std::size_t, Bs>{}),
        ...);
    }(std::make_index_sequence<64>{});
    // clang-format on
}

} // namespace piejam::numeric::bit::test
