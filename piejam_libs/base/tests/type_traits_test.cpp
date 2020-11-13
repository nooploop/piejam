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

#include <piejam/type_traits.h>

#include <gtest/gtest.h>

namespace piejam::test
{

struct default_ctor_except
{
    default_ctor_except() noexcept(false) {}
};

TEST(type_traits, is_nothrow_default_constructible)
{
    static_assert(is_nothrow_default_constructible_v<float>);
    static_assert(!is_nothrow_default_constructible_v<default_ctor_except>);
    static_assert(is_nothrow_default_constructible_v<float, int, char>);
    static_assert(!is_nothrow_default_constructible_v<
                  float,
                  int,
                  char,
                  default_ctor_except>);
}

} // namespace piejam::meta::test
