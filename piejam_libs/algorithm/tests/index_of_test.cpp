// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/index_of.h>

#include <gtest/gtest.h>

#include <array>
#include <utility>

namespace piejam::algorithm::test
{

TEST(index_of, returns_index_of_found_item)
{
    std::array arr{5, 58, 23, 77, 123};
    EXPECT_EQ(2u, index_of(arr, 23));
}

TEST(index_of, returns_npos_if_item_not_found)
{
    std::array arr{5, 58, 23, 77, 123};
    EXPECT_EQ(npos, index_of(arr, 99));
}

TEST(index_of, constexpr_found_item)
{
    static_assert(2u == index_of(std::array{5, 58, 23, 77, 123}, 23));
}

TEST(index_of, constexpr_not_found_item)
{
    static_assert(npos == index_of(std::array{5, 58, 23, 77, 123}, 99));
}

TEST(index_of_if, returns_index_of_found_item)
{
    std::array arr{
            std::pair('x', 3.f),
            std::pair('y', 5.f),
            std::pair('z', 7.f)};
    EXPECT_EQ(1u, index_of_if(arr, [](auto const& p) {
                  return p.first == 'y';
              }));
    EXPECT_EQ(2u, index_of_if(arr, [](auto const& p) {
                  return p.second > 5.f;
              }));
}

TEST(index_of_if, returns_npos_if_item_not_found)
{
    std::array arr{
            std::pair('x', 3.f),
            std::pair('y', 5.f),
            std::pair('z', 7.f)};
    EXPECT_EQ(npos, index_of_if(arr, [](auto const& p) {
                  return p.first == 'a';
              }));
}

TEST(index_of_if, constexpr_found_item)
{
    static_assert(2u == index_of_if(std::array{5, 23, 0}, [](auto x) {
                      return x == 0;
                  }));
}

TEST(index_of_if, constexpr_not_found_item)
{
    static_assert(npos == index_of_if(std::array{5, 23, 0}, [](auto x) {
                      return x == 1;
                  }));
}

} // namespace piejam::algorithm::test
