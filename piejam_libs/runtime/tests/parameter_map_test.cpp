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

    auto v = sut.get(id);
    EXPECT_EQ(1.f, v);
}

TEST(parameter_map, add_with_value)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{}, 2.f);

    auto v = sut.get(id);
    EXPECT_EQ(2.f, v);
}

TEST(parameter_map, add_remove)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{});
    ASSERT_TRUE(sut.contains(id));
    sut.remove(id);

    EXPECT_FALSE(sut.contains(id));
}

TEST(parameter_map, get_non_existing_parameter)
{
    parameter::map<float_parameter> sut;

    EXPECT_EQ(
            nullptr,
            sut.get_parameter(parameter::id_t<float_parameter>::generate()));
}

TEST(parameter_map, get_existing_parameter)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{});
    EXPECT_NE(nullptr, sut.get_parameter(id));
}

TEST(parameter_map, find_for_non_existing_parameter)
{
    parameter::map<float_parameter> sut;

    EXPECT_EQ(nullptr, sut.find(parameter::id_t<float_parameter>::generate()));
}

TEST(parameter_map, find_for_existing_parameter)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{}, 23.f);
    auto value = sut.find(id);
    ASSERT_NE(nullptr, value);
    EXPECT_EQ(23.f, *value);
}

TEST(parameter_map, get_cached_for_non_existing_parameter)
{
    parameter::map<float_parameter> sut;

    EXPECT_EQ(
            nullptr,
            sut.get_cached(parameter::id_t<float_parameter>::generate()));
}

TEST(parameter_map, get_cached_for_existing_parameter)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{}, 23.f);
    auto value = sut.get_cached(id);
    ASSERT_NE(nullptr, value);
    EXPECT_EQ(23.f, *value);
}

TEST(parameter_map, set_value)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{});

    sut.set(id, 2.f);
    auto v = sut.get(id);
    EXPECT_EQ(2.f, v);
}

TEST(parameter_map, cached_is_updated_after_set)
{
    parameter::map<float_parameter> sut;

    auto id = sut.add(float_parameter{});

    auto cached = sut.get_cached(id);
    ASSERT_NE(nullptr, cached);
    EXPECT_EQ(1.f, *cached);

    sut.set(id, 2.f);

    EXPECT_EQ(2.f, *cached);
}

TEST(parameter_map, set_values)
{
    parameter::map<float_parameter> sut;
    auto id1 = sut.add(float_parameter{});
    auto id2 = sut.add(float_parameter{});
    auto id3 = sut.add(float_parameter{});

    parameter::id_value_map_t<float_parameter> new_values = {
            {id2, 2.f},
            {id3, 3.f}};

    sut.set(new_values);

    EXPECT_EQ(1.f, sut.get(id1));
    EXPECT_EQ(2.f, sut.get(id2));
    EXPECT_EQ(3.f, sut.get(id3));
}

} // namespace piejam::runtime::parameter::test
