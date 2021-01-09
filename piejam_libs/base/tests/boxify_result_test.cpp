// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/boxify_result.h>

#include <gtest/gtest.h>

namespace piejam
{

TEST(boxify_result, can_get_args)
{
    auto f = boxify_result([](int const& x, int& y) -> int { return x + y; });
    static_assert(std::is_same_v<
                  std::tuple<int const&, int&>,
                  boost::callable_traits::args_t<decltype(f)>>);
    static_assert(std::is_same_v<
                  box<int>,
                  boost::callable_traits::return_type_t<decltype(f)>>);
}

} // namespace piejam
