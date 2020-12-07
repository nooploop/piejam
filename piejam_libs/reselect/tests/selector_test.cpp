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

#include <piejam/reselect/selector.h>

#include <gtest/gtest.h>

namespace piejam::reselect::test
{

namespace
{

struct state
{
    int x{};
};

} // namespace

TEST(selector, make_selector_from_pmd)
{
    auto sut = selector<int, state>(&state::x);
    state st{5};
    EXPECT_EQ(5, sut.get(st));
}

TEST(selector, make_selector_from_lambda)
{
    auto sut = selector<int, state>([](state const& st) { return st.x + 5; });
    state st{5};
    EXPECT_EQ(10, sut.get(st));
}

} // namespace piejam::reselect::test
