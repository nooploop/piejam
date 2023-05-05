// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/stream_ring_buffer.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <array>

namespace piejam::audio::engine::test
{

TEST(stream_ring_buffer, write_all_samples_if_enough_space)
{
    stream_ring_buffer buf(1, 8);

    std::array ch0_data{0.f, 1.f, 2.f, 3.f};
    audio_slice slice0{ch0_data};

    std::array inputs{std::cref(slice0)};

    EXPECT_EQ(4u, buf.write(inputs, 4));
}

TEST(stream_ring_buffer, write_only_until_max_capacity)
{
    stream_ring_buffer buf(1, 2);

    std::array ch0_data{0.f, 1.f, 2.f, 3.f};
    audio_slice slice0{ch0_data};

    std::array inputs{std::cref(slice0)};

    EXPECT_EQ(2u, buf.write(inputs, 4));
}

TEST(stream_ring_buffer,
     write_only_until_max_capacity_on_consecutive_writes_on_border)
{
    stream_ring_buffer buf(1, 4);

    std::array ch0_data{0.f, 1.f, 2.f, 3.f};
    audio_slice slice0{ch0_data};

    std::array inputs{std::cref(slice0)};

    EXPECT_EQ(4u, buf.write(inputs, 4));
    EXPECT_EQ(0u, buf.write(inputs, 4));
}

TEST(stream_ring_buffer, write_only_until_max_capacity_on_consecutive_writes)
{
    stream_ring_buffer buf(1, 6);

    std::array ch0_data{0.f, 1.f, 2.f, 3.f};
    audio_slice slice0{ch0_data};

    std::array inputs{std::cref(slice0)};

    EXPECT_EQ(4u, buf.write(inputs, 4));
    EXPECT_EQ(2u, buf.write(inputs, 4));
}

TEST(stream_ring_buffer, consume_multi_channel)
{
    stream_ring_buffer buf(2, 8);

    std::array ch0_data{0.f, 1.f, 2.f, 3.f};

    audio_slice slice0{ch0_data};
    audio_slice slice1{23.f};

    std::array inputs{std::cref(slice0), std::cref(slice1)};

    ASSERT_EQ(4u, buf.write(inputs, 4));

    auto result = buf.consume();

    ASSERT_EQ(2, result.num_channels());
    EXPECT_THAT(result.channels()[0], testing::ElementsAreArray(ch0_data));
    EXPECT_THAT(
            result.channels()[1],
            testing::ElementsAre(23.f, 23.f, 23.f, 23.f));
}

TEST(stream_ring_buffer, consume_on_border_will_call_the_functor_twice)
{
    stream_ring_buffer buf(2, 6);

    std::array<float, 4> ch0_data{};
    std::array<float, 4> ch1_data{};

    audio_slice slice0{ch0_data};
    audio_slice slice1{ch1_data};

    std::array inputs{std::cref(slice0), std::cref(slice1)};

    EXPECT_EQ(4u, buf.write(inputs, 4));
    buf.consume();

    ch0_data.fill(1.f);
    ch1_data.fill(2.f);

    EXPECT_EQ(4u, buf.write(inputs, 4));

    auto result = buf.consume();

    ASSERT_EQ(2u, result.num_channels());
    EXPECT_THAT(result.channels()[0], testing::ElementsAreArray(ch0_data));
    EXPECT_THAT(result.channels()[1], testing::ElementsAreArray(ch1_data));
}

TEST(stream_ring_buffer,
     write_to_full_capacity_consume_on_border_will_call_the_functor_twice)
{
    stream_ring_buffer buf(2, 6);

    std::array<float, 7> ch0_data{};
    std::array<float, 7> ch1_data{};

    audio_slice slice0{ch0_data};
    audio_slice slice1{ch1_data};

    std::array inputs{std::cref(slice0), std::cref(slice1)};

    EXPECT_EQ(4u, buf.write(inputs, 4));
    buf.consume();

    ch0_data = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
    ch1_data = {6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f};

    EXPECT_EQ(6u, buf.write(inputs, 7));

    auto result = buf.consume();

    ASSERT_EQ(2u, result.num_channels());
    // ring-buffer was full, last element was dropped
    EXPECT_THAT(
            result.channels()[0],
            testing::ElementsAre(0.f, 1.f, 2.f, 3.f, 4.f, 5.f));
    EXPECT_THAT(
            result.channels()[1],
            testing::ElementsAre(6.f, 5.f, 4.f, 3.f, 2.f, 1.f));
}

TEST(stream_ring_buffer, write_over_full_capacity)
{
    stream_ring_buffer buf(2, 6);

    std::array<float, 7> ch0_data{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
    std::array<float, 7> ch1_data{6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f};

    audio_slice slice0{ch0_data};
    audio_slice slice1{ch1_data};

    std::array inputs{std::cref(slice0), std::cref(slice1)};

    EXPECT_EQ(6u, buf.write(inputs, 7));
    EXPECT_EQ(0u, buf.write(inputs, 7));

    auto result = buf.consume();

    ASSERT_EQ(2u, result.num_channels());
    // ring-buffer was full, last element was dropped
    EXPECT_THAT(
            result.channels()[0],
            testing::ElementsAre(0.f, 1.f, 2.f, 3.f, 4.f, 5.f));
    EXPECT_THAT(
            result.channels()[1],
            testing::ElementsAre(6.f, 5.f, 4.f, 3.f, 2.f, 1.f));
}

} // namespace piejam::audio::engine::test
