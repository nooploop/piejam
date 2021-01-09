// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
