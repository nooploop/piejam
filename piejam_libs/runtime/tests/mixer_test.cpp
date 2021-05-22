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
    auto bus1 = channels.add(channel{});
    auto bus2 = channels.add(channel{});

    auto vs1 = valid_source_channels(channels, bus1);
    EXPECT_TRUE(Matches(ElementsAre(bus2))(vs1));

    auto vs2 = valid_source_channels(channels, bus2);
    EXPECT_TRUE(Matches(ElementsAre(bus1))(vs2));

    auto vt1 = valid_target_channels(channels, bus1);
    EXPECT_TRUE(Matches(ElementsAre(bus2))(vt1));

    auto vt2 = valid_target_channels(channels, bus2);
    EXPECT_TRUE(Matches(ElementsAre(bus1))(vt2));
}

TEST(mixer_valid_io, test2)
{
    using testing::ElementsAre;
    using testing::Matches;

    channels_t channels;
    auto bus1 = channels.add(channel{});
    auto bus2 = channels.add(
            channel{.in = bus1,
                    .volume = {},
                    .pan_balance = {},
                    .record = {},
                    .mute = {},
                    .solo = {},
                    .level = {}});

    auto vs1 = valid_source_channels(channels, bus1);
    EXPECT_TRUE(vs1.empty());

    auto vs2 = valid_source_channels(channels, bus2);
    EXPECT_TRUE(Matches(ElementsAre(bus1))(vs2));

    auto vt1 = valid_target_channels(channels, bus1);
    EXPECT_TRUE(Matches(ElementsAre(bus2))(vt1));

    auto vt2 = valid_target_channels(channels, bus2);
    EXPECT_TRUE(vt2.empty());
}

} // namespace piejam::runtime::mixer::test
