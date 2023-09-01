// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/entity_id.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameters_map.h>

#include <gtest/gtest.h>

namespace piejam::runtime::parameter::test
{

struct float_parameter
{
    using value_type = float;

    float default_value{1.f};
};

TEST(parameters_map, add_default_get)
{
    parameters_map sut;

    auto id = id_t<float_parameter>::generate();
    sut.emplace(id);

    EXPECT_EQ(1.f, sut[id].value.get());
}

TEST(parameters_map, add_remove)
{
    parameters_map sut;

    auto id = id_t<float_parameter>::generate();
    sut.emplace(id);
    ASSERT_TRUE(sut.contains(id));
    sut.remove(id);

    EXPECT_FALSE(sut.contains(id));
}

TEST(parameters_map, get_existing_parameter)
{
    parameters_map sut;

    auto id = id_t<float_parameter>::generate();
    sut.emplace(id);
    EXPECT_NE(nullptr, sut.find(id));
}

TEST(parameters_map, find_for_non_existing_parameter)
{
    parameters_map sut;

    EXPECT_EQ(nullptr, sut.find(parameter::id_t<float_parameter>::generate()));
}

TEST(parameters_map, get_cached_for_existing_parameter)
{
    parameters_map sut;

    auto id = id_t<float_parameter>::generate();
    sut.emplace(id);
    auto desc = sut.find(id);
    ASSERT_NE(nullptr, desc);
    EXPECT_EQ(1.f, *desc->value.cached());
}

TEST(parameters_map, set_value)
{
    parameters_map sut;

    auto id = id_t<float_parameter>::generate();
    sut.emplace(id);

    sut[id].value.set(2.f);
    EXPECT_EQ(2.f, sut[id].value.get());
}

TEST(parameters_map, cached_is_updated_after_set)
{
    parameters_map sut;

    auto id = id_t<float_parameter>::generate();
    sut.emplace(id);

    auto cached = sut[id].value.cached();
    ASSERT_NE(nullptr, cached);
    EXPECT_EQ(1.f, *cached);

    sut[id].value.set(2.f);

    EXPECT_EQ(2.f, *cached);
}

} // namespace piejam::runtime::parameter::test
