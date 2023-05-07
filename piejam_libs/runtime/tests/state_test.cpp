// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/state.h>

#include <piejam/runtime/parameter_maps_access.h>

#include <gtest/gtest.h>

namespace piejam::runtime::test
{

struct state_with_one_mixer_input : testing::Test
{
    state_with_one_mixer_input()
        : bus_id(add_mixer_channel(sut, "foo", audio::bus_type::mono))
    {
    }

    state sut;
    mixer::channel_id bus_id;
    mixer::channel const* bus{};
};

TEST_F(state_with_one_mixer_input, after_add_mixer_channel)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());

    auto const& bus = sut.mixer_state.channels[bus_id];

    EXPECT_EQ("foo", *bus.name);

    auto const& volume_param = get_parameter(sut.params, bus.volume);
    EXPECT_EQ(1.f, volume_param.default_value);
    EXPECT_EQ(0.f, volume_param.min);
    EXPECT_EQ(4.f, volume_param.max);
    auto volume_value = get_parameter_value(sut.params, bus.volume);
    EXPECT_EQ(1.f, volume_value);

    auto const& pan_balance_param = get_parameter(sut.params, bus.pan_balance);
    EXPECT_EQ(0.f, pan_balance_param.default_value);
    EXPECT_EQ(-1.f, pan_balance_param.min);
    EXPECT_EQ(1.f, pan_balance_param.max);
    auto pan_balance_value = get_parameter_value(sut.params, bus.pan_balance);
    EXPECT_EQ(0.f, pan_balance_value);

    auto const& mute_param = get_parameter(sut.params, bus.mute);
    EXPECT_EQ(false, mute_param.default_value);
    auto mute_value = get_parameter_value(sut.params, bus.mute);
    EXPECT_EQ(false, mute_value);

    auto const& level_param = get_parameter(sut.params, bus.level);
    EXPECT_EQ(stereo_level{}, level_param.default_value);
    auto level_value = get_parameter_value(sut.params, bus.level);
    EXPECT_EQ(0.f, level_value.left);
    EXPECT_EQ(0.f, level_value.right);
}

TEST_F(state_with_one_mixer_input,
       removing_mixer_channel_removes_also_its_parameters)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());
    ASSERT_EQ(2u, size<float_parameter>(sut.params));
    ASSERT_EQ(3u, size<bool_parameter>(sut.params));
    ASSERT_EQ(0u, size<int_parameter>(sut.params));
    ASSERT_EQ(1u, size<stereo_level_parameter>(sut.params));

    remove_mixer_channel(sut, bus_id);

    EXPECT_TRUE(sut.mixer_state.inputs->empty());
    EXPECT_EQ(nullptr, sut.mixer_state.channels.find(bus_id));
    EXPECT_TRUE(empty<float_parameter>(sut.params));
    EXPECT_TRUE(empty<bool_parameter>(sut.params));
    EXPECT_TRUE(empty<stereo_level_parameter>(sut.params));
}

struct state_with_one_fx : testing::Test
{
    state_with_one_fx()
        : bus_id(add_mixer_channel(sut, "foo", audio::bus_type::stereo))
        , fx_mod_id(insert_internal_fx_module(
                  sut,
                  bus_id,
                  0,
                  fx::internal::tool,
                  {},
                  {}))
    {
    }

    state sut;
    mixer::channel_id bus_id;
    fx::module_id fx_mod_id;
};

TEST_F(state_with_one_fx, after_insert_fx)
{
    EXPECT_EQ(1u, sut.fx_modules.size());
    EXPECT_TRUE(sut.fx_modules.contains(fx_mod_id));
}

TEST_F(state_with_one_fx, remove_fx)
{
    EXPECT_EQ(1u, sut.fx_modules.size());
    ASSERT_TRUE(sut.fx_modules.contains(fx_mod_id));

    remove_fx_module(sut, fx_mod_id);

    EXPECT_TRUE(sut.fx_modules.empty());
    EXPECT_FALSE(sut.fx_modules.contains(fx_mod_id));
}

} // namespace piejam::runtime::test
