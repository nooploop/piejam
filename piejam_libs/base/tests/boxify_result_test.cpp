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
