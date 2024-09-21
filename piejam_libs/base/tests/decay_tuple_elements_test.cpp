// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/tuple.h>

#include <gtest/gtest.h>

namespace piejam::tuple::test
{

TEST(decay_tuple_elements, verify)
{
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_elements_t<std::tuple<int, int>>>);
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_elements_t<std::tuple<int const&, int>>>);
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_elements_t<std::tuple<int&, int>>>);
    static_assert(std::is_same_v<
                  std::tuple<int, int>,
                  decay_elements_t<std::tuple<int&&, int>>>);
    static_assert(std::is_same_v<std::tuple<>, decay_elements_t<std::tuple<>>>);
}

} // namespace piejam::tuple::test
