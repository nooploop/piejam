// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/state.h>

#include <piejam/math.h>
#include <piejam/runtime/parameter/map.h>

#include <gtest/gtest.h>

namespace piejam::runtime::test
{

struct state_with_one_mixer_input : testing::Test
{
    state_with_one_mixer_input()
        : channel_id(add_mixer_channel(sut, "foo", audio::bus_type::mono))
    {
    }

    state sut;
    mixer::channel_id channel_id;
    mixer::channel const* channel{};
};

TEST_F(state_with_one_mixer_input, after_add_mixer_channel)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());

    auto const& channel = sut.mixer_state.channels[channel_id];

    EXPECT_EQ("foo", *channel.name);

    auto const& volume_param = sut.params[channel.volume].param;
    EXPECT_EQ(1.f, volume_param.default_value);
    EXPECT_EQ(0.f, volume_param.min);
    EXPECT_EQ(math::from_dB(6.f), volume_param.max);
    auto volume_value = sut.params[channel.volume].value.get();
    EXPECT_EQ(1.f, volume_value);

    auto const& pan_balance_param = sut.params[channel.pan_balance].param;
    EXPECT_EQ(0.f, pan_balance_param.default_value);
    EXPECT_EQ(-1.f, pan_balance_param.min);
    EXPECT_EQ(1.f, pan_balance_param.max);
    auto pan_balance_value = sut.params[channel.pan_balance].value.get();
    EXPECT_EQ(0.f, pan_balance_value);

    auto const& mute_param = sut.params[channel.mute].param;
    EXPECT_EQ(false, mute_param.default_value);
    auto mute_value = sut.params[channel.mute].value.get();
    EXPECT_EQ(false, mute_value);

    auto const& level_param = sut.params[channel.peak_level].param;
    EXPECT_EQ(stereo_level{}, level_param.default_value);
    auto level_value = sut.params[channel.peak_level].value.get();
    EXPECT_EQ(0.f, level_value.left);
    EXPECT_EQ(0.f, level_value.right);
}

TEST_F(state_with_one_mixer_input,
       removing_mixer_channel_removes_also_its_parameters)
{
    ASSERT_EQ(1u, sut.mixer_state.inputs->size());
    ASSERT_EQ(2u, sut.params.get_map<float_parameter>().size());
    ASSERT_EQ(3u, sut.params.get_map<bool_parameter>().size());
    ASSERT_EQ(0u, sut.params.get_map<int_parameter>().size());
    ASSERT_EQ(2u, sut.params.get_map<stereo_level_parameter>().size());

    remove_mixer_channel(sut, channel_id);

    EXPECT_TRUE(sut.mixer_state.inputs->empty());
    EXPECT_EQ(nullptr, sut.mixer_state.channels.find(channel_id));
    EXPECT_TRUE(sut.params.get_map<float_parameter>().empty());
    EXPECT_TRUE(sut.params.get_map<bool_parameter>().empty());
    EXPECT_TRUE(sut.params.get_map<stereo_level_parameter>().empty());
}

} // namespace piejam::runtime::test
