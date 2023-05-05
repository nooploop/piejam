// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/multichannel_buffer.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(multichannel_buffer, non_interleaved_channels)
{
    multichannel_buffer buf{
            multichannel_layout::non_interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto channels = buf.channels();
    ASSERT_EQ(2, channels.major_size());
    EXPECT_THAT(channels[0], testing::ElementsAre(1.f, 2.f, 3.f));
    EXPECT_THAT(channels[1], testing::ElementsAre(4.f, 5.f, 6.f));
}

TEST(multichannel_buffer, non_interleaved_channels_after_cast)
{
    multichannel_buffer buf{
            multichannel_layout::non_interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto channels = buf.view()
                            .cast<multichannel_layout_non_interleaved, 2>()
                            .channels();
    ASSERT_EQ(2, channels.major_size());
    EXPECT_THAT(channels[0], testing::ElementsAre(1.f, 2.f, 3.f));
    EXPECT_THAT(channels[1], testing::ElementsAre(4.f, 5.f, 6.f));
}

TEST(multichannel_buffer, interleaved_channels)
{
    multichannel_buffer buf{
            multichannel_layout::interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto channels = buf.channels();
    ASSERT_EQ(2, channels.major_size());
    EXPECT_THAT(channels[0], testing::ElementsAre(1.f, 3.f, 5.f));
    EXPECT_THAT(channels[1], testing::ElementsAre(2.f, 4.f, 6.f));
}

TEST(multichannel_buffer, interleaved_channels_after_cast)
{
    multichannel_buffer buf{
            multichannel_layout::interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto channels =
            buf.view().cast<multichannel_layout_interleaved, 2>().channels();
    ASSERT_EQ(2, channels.major_size());
    EXPECT_THAT(channels[0], testing::ElementsAre(1.f, 3.f, 5.f));
    EXPECT_THAT(channels[1], testing::ElementsAre(2.f, 4.f, 6.f));
}

TEST(multichannel_buffer, non_interleaved_frames)
{
    multichannel_buffer buf{
            multichannel_layout::non_interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto frames = buf.frames();
    ASSERT_EQ(3, frames.major_size());
    EXPECT_THAT(frames[0], testing::ElementsAre(1.f, 4.f));
    EXPECT_THAT(frames[1], testing::ElementsAre(2.f, 5.f));
    EXPECT_THAT(frames[2], testing::ElementsAre(3.f, 6.f));
}

TEST(multichannel_buffer, non_interleaved_frames_after_cast)
{
    multichannel_buffer buf{
            multichannel_layout::non_interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto frames =
            buf.view().cast<multichannel_layout_non_interleaved, 2>().frames();
    ASSERT_EQ(3, frames.major_size());
    EXPECT_THAT(frames[0], testing::ElementsAre(1.f, 4.f));
    EXPECT_THAT(frames[1], testing::ElementsAre(2.f, 5.f));
    EXPECT_THAT(frames[2], testing::ElementsAre(3.f, 6.f));
}

TEST(multichannel_buffer, interleaved_frames)
{
    multichannel_buffer buf{
            multichannel_layout::interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto frames = buf.frames();
    ASSERT_EQ(3, frames.major_size());
    EXPECT_THAT(frames[0], testing::ElementsAre(1.f, 2.f));
    EXPECT_THAT(frames[1], testing::ElementsAre(3.f, 4.f));
    EXPECT_THAT(frames[2], testing::ElementsAre(5.f, 6.f));
}

TEST(multichannel_buffer, interleaved_frames_after_cast)
{
    multichannel_buffer buf{
            multichannel_layout::interleaved,
            2,
            std::vector{1.f, 2.f, 3.f, 4.f, 5.f, 6.f}};

    auto frames =
            buf.view().cast<multichannel_layout_interleaved, 2>().frames();
    ASSERT_EQ(3, frames.major_size());
    EXPECT_THAT(frames[0], testing::ElementsAre(1.f, 2.f));
    EXPECT_THAT(frames[1], testing::ElementsAre(3.f, 4.f));
    EXPECT_THAT(frames[2], testing::ElementsAre(5.f, 6.f));
}

} // namespace piejam::audio::test
