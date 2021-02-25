// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/state.h>

#include <gtest/gtest.h>

namespace piejam::runtime::test
{

struct state_with_one_mixer_input : testing::Test
{
    state_with_one_mixer_input()
        : bus_id(add_mixer_bus(sut, "foo"))
    {
    }

    state sut;
    mixer::bus_id bus_id;
    mixer::bus const* bus{};
};

TEST_F(state_with_one_mixer_input, after_add_mixer_bus)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());

    auto bus = sut.mixer_state.buses[bus_id];
    ASSERT_NE(nullptr, bus);

    EXPECT_EQ("foo", bus->name);

    auto volume_param = sut.params.get_parameter(bus->volume);
    ASSERT_NE(nullptr, volume_param);
    EXPECT_EQ(1.f, volume_param->default_value);
    EXPECT_EQ(0.f, volume_param->min);
    EXPECT_EQ(4.f, volume_param->max);
    auto volume_value = sut.params.get(bus->volume);
    ASSERT_NE(nullptr, volume_value);
    EXPECT_EQ(1.f, *volume_value);

    auto pan_balance_param = sut.params.get_parameter(bus->pan_balance);
    ASSERT_NE(nullptr, pan_balance_param);
    EXPECT_EQ(0.f, pan_balance_param->default_value);
    EXPECT_EQ(-1.f, pan_balance_param->min);
    EXPECT_EQ(1.f, pan_balance_param->max);
    auto pan_balance_value = sut.params.get(bus->pan_balance);
    ASSERT_NE(nullptr, pan_balance_value);
    EXPECT_EQ(0.f, *pan_balance_value);

    auto mute_param = sut.params.get_parameter(bus->mute);
    ASSERT_NE(nullptr, mute_param);
    EXPECT_EQ(false, mute_param->default_value);
    auto mute_value = sut.params.get(bus->mute);
    ASSERT_NE(nullptr, mute_param);
    EXPECT_EQ(false, *mute_value);

    auto level_param = sut.params.get_parameter(bus->level);
    ASSERT_NE(nullptr, level_param);
    EXPECT_EQ(stereo_level{}, level_param->default_value);
    auto level_value = sut.params.get(bus->level);
    ASSERT_NE(nullptr, level_param);
    EXPECT_EQ(0.f, level_value->left);
    EXPECT_EQ(0.f, level_value->right);
}

TEST_F(state_with_one_mixer_input, remove_mixer_bus)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());
    ASSERT_EQ(2u, sut.params.get_map<float_parameter>().size());
    ASSERT_EQ(1u, sut.params.get_map<bool_parameter>().size());
    ASSERT_EQ(1u, sut.params.get_map<stereo_level_parameter>().size());

    remove_mixer_bus(sut, bus_id);

    EXPECT_TRUE(sut.mixer_state.inputs->empty());
    EXPECT_EQ(nullptr, sut.mixer_state.buses[bus_id]);
    EXPECT_TRUE(sut.params.get_map<float_parameter>().empty());
    EXPECT_TRUE(sut.params.get_map<bool_parameter>().empty());
    EXPECT_TRUE(sut.params.get_map<stereo_level_parameter>().empty());
}

TEST_F(state_with_one_mixer_input, clear_mixer_buses)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());
    ASSERT_EQ(2u, sut.params.get_map<float_parameter>().size());
    ASSERT_EQ(1u, sut.params.get_map<bool_parameter>().size());
    ASSERT_EQ(1u, sut.params.get_map<stereo_level_parameter>().size());

    //    clear_mixer_buses<io_direction::input>(sut);

    EXPECT_TRUE(sut.mixer_state.inputs->empty());
    EXPECT_EQ(nullptr, sut.mixer_state.buses[bus_id]);
    EXPECT_TRUE(sut.params.get_map<float_parameter>().empty());
    EXPECT_TRUE(sut.params.get_map<bool_parameter>().empty());
    EXPECT_TRUE(sut.params.get_map<stereo_level_parameter>().empty());
}

TEST_F(state_with_one_mixer_input, clear_mixer_buses_other_direction)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());
    //    EXPECT_TRUE(sut.mixer_state.outputs->empty());
    ASSERT_EQ(2u, sut.params.get_map<float_parameter>().size());
    ASSERT_EQ(1u, sut.params.get_map<bool_parameter>().size());
    ASSERT_EQ(1u, sut.params.get_map<stereo_level_parameter>().size());

    //    clear_mixer_buses<io_direction::output>(sut);

    EXPECT_EQ(1u, sut.mixer_state.inputs->size());
    EXPECT_NE(nullptr, sut.mixer_state.buses[bus_id]);
    //    EXPECT_TRUE(sut.mixer_state.outputs->empty());
    EXPECT_EQ(2u, sut.params.get_map<float_parameter>().size());
    EXPECT_EQ(1u, sut.params.get_map<bool_parameter>().size());
    EXPECT_EQ(1u, sut.params.get_map<stereo_level_parameter>().size());
}

struct state_with_one_fx : testing::Test
{
    state_with_one_fx()
        : bus_id(add_mixer_bus(sut, "foo"))
        , fx_mod_id(insert_internal_fx_module(
                  sut,
                  bus_id,
                  0,
                  fx::internal::gain,
                  {},
                  {}))
    {
    }

    state sut;
    mixer::bus_id bus_id;
    fx::module_id fx_mod_id;
};

TEST_F(state_with_one_fx, after_insert_fx)
{
    EXPECT_EQ(1u, sut.fx_modules.size());
    ASSERT_NE(nullptr, sut.fx_modules[fx_mod_id]);
}

TEST_F(state_with_one_fx, remove_fx)
{
    EXPECT_EQ(1u, sut.fx_modules.size());
    ASSERT_NE(nullptr, sut.fx_modules[fx_mod_id]);

    remove_fx_module(sut, fx_mod_id);

    EXPECT_TRUE(sut.fx_modules.empty());
    EXPECT_EQ(nullptr, sut.fx_modules[fx_mod_id]);
}

TEST_F(state_with_one_fx, remove_mixer_bus)
{
    EXPECT_EQ(1u, sut.fx_modules.size());
    ASSERT_NE(nullptr, sut.fx_modules[fx_mod_id]);

    remove_mixer_bus(sut, bus_id);

    EXPECT_TRUE(sut.fx_modules.empty());
    EXPECT_EQ(nullptr, sut.fx_modules[fx_mod_id]);
}

TEST_F(state_with_one_fx, clear_mixer_buses)
{
    EXPECT_EQ(1u, sut.fx_modules.size());
    ASSERT_NE(nullptr, sut.fx_modules[fx_mod_id]);

    //    clear_mixer_buses<io_direction::input>(sut);

    EXPECT_TRUE(sut.fx_modules.empty());
    EXPECT_EQ(nullptr, sut.fx_modules[fx_mod_id]);
}

TEST_F(state_with_one_fx, clear_mixer_buses_other_direction)
{
    EXPECT_EQ(1u, sut.fx_modules.size());
    ASSERT_NE(nullptr, sut.fx_modules[fx_mod_id]);

    //    clear_mixer_buses<io_direction::output>(sut);

    EXPECT_EQ(1u, sut.fx_modules.size());
    EXPECT_NE(nullptr, sut.fx_modules[fx_mod_id]);
}

} // namespace piejam::runtime::test
