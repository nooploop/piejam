// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/mixer.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

namespace piejam::runtime::mixer::test
{

TEST(mixer_valid_io, test1)
{
    using testing::ElementsAre;
    using testing::Matches;

    channels_t channels;
    auto bus1 = channels.add(channel{
            .name = "foo",
            .bus_type = audio::bus_type::stereo,
            .volume = {},
            .pan_balance = {},
            .record = {},
            .mute = {},
            .solo = {},
            .peak_level = {},
            .rms_level = {},
    });
    auto bus2 = channels.add(channel{
            .name = "bar",
            .bus_type = audio::bus_type::stereo,
            .volume = {},
            .pan_balance = {},
            .record = {},
            .mute = {},
            .solo = {},
            .peak_level = {},
            .rms_level = {},
    });

    auto vs1 = valid_channels(io_socket::in, channels, bus1);
    EXPECT_TRUE(Matches(ElementsAre(bus2))(vs1));

    auto vs2 = valid_channels(io_socket::in, channels, bus2);
    EXPECT_TRUE(Matches(ElementsAre(bus1))(vs2));

    auto vt1 = valid_channels(io_socket::out, channels, bus1);
    EXPECT_TRUE(Matches(ElementsAre(bus2))(vt1));

    auto vt2 = valid_channels(io_socket::out, channels, bus2);
    EXPECT_TRUE(Matches(ElementsAre(bus1))(vt2));
}

TEST(mixer_valid_io, test2)
{
    using testing::ElementsAre;
    using testing::Matches;

    channels_t channels;
    auto bus1 = channels.add(channel{});
    auto bus2 = channels.add(channel{
            .name = "foo",
            .bus_type = audio::bus_type::stereo,
            .in = bus1,
            .volume = {},
            .pan_balance = {},
            .record = {},
            .mute = {},
            .solo = {},
            .peak_level = {},
            .rms_level = {},
    });

    auto vs1 = valid_channels(io_socket::in, channels, bus1);
    EXPECT_TRUE(vs1.empty());

    auto vs2 = valid_channels(io_socket::in, channels, bus2);
    EXPECT_TRUE(Matches(ElementsAre(bus1))(vs2));

    auto vt1 = valid_channels(io_socket::out, channels, bus1);
    EXPECT_TRUE(Matches(ElementsAre(bus2))(vt1));

    auto vt2 = valid_channels(io_socket::out, channels, bus2);
    EXPECT_TRUE(vt2.empty());
}

} // namespace piejam::runtime::mixer::test
