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

#include <piejam/runtime/parameter/map.h>

#include <gtest/gtest.h>

namespace piejam::runtime::parameter::test
{

struct float_parameter
{
    using value_type = float;

    float default_value{1.f};
};

TEST(parameter_map, add_default_get)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{});

    auto* v = sut.get(id);
    ASSERT_NE(nullptr, v);
    EXPECT_EQ(1.f, *v);
}

TEST(parameter_map, add_with_value)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{}, 2.f);

    auto* v = sut.get(id);
    ASSERT_NE(nullptr, v);
    EXPECT_EQ(2.f, *v);
}

TEST(parameter_map, add_remove)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{});
    ASSERT_TRUE(sut.contains(id));
    sut.remove(id);

    EXPECT_FALSE(sut.contains(id));
}

} // namespace piejam::runtime::parameter::test
