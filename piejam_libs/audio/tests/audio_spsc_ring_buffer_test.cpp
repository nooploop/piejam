// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/audio_spsc_ring_buffer.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>

namespace piejam::audio::engine::test
{

TEST(audio_spsc_ring_buffer, write_all_samples_if_enough_space)
{
    audio_spsc_ring_buffer buf(8);

    EXPECT_EQ(4u, buf.write(std::array{0.f, 1.f, 2.f, 3.f}));
}

TEST(audio_spsc_ring_buffer, write_only_until_max_capacity)
{
    audio_spsc_ring_buffer buf(2);

    EXPECT_EQ(2u, buf.write(std::array{0.f, 1.f, 2.f, 3.f}));
}

TEST(audio_spsc_ring_buffer,
     write_only_until_max_capacity_on_consecutive_writes_on_border)
{
    audio_spsc_ring_buffer buf(4);

    EXPECT_EQ(4u, buf.write(std::array{0.f, 1.f, 2.f, 3.f}));
    EXPECT_EQ(0u, buf.write(std::array{0.f, 1.f, 2.f, 3.f}));
}

TEST(audio_spsc_ring_buffer,
     write_only_until_max_capacity_on_consecutive_writes)
{
    audio_spsc_ring_buffer buf(6);

    EXPECT_EQ(4u, buf.write(std::array{0.f, 1.f, 2.f, 3.f}));
    EXPECT_EQ(2u, buf.write(std::array{0.f, 1.f, 2.f, 3.f}));
}

TEST(audio_spsc_ring_buffer, consume_from_empty_does_nothing)
{
    audio_spsc_ring_buffer buf(8);

    std::array data{0.f, 1.f, 2.f, 3.f};

    ASSERT_EQ(4u, buf.write(data));

    std::size_t num_calls{};
    buf.consume([&](std::span<float const> const d) {
        EXPECT_TRUE(std::ranges::equal(d, data));
        ++num_calls;
    });

    EXPECT_EQ(1u, num_calls);
}

TEST(audio_spsc_ring_buffer, consume_on_border_will_call_the_functor_twice)
{
    audio_spsc_ring_buffer buf(6);

    ASSERT_EQ(4u, buf.write(std::array{0.f, 0.f, 0.f, 0.f}));
    buf.consume([](auto const&) {});

    std::array data{0.f, 1.f, 2.f, 3.f};

    ASSERT_EQ(4u, buf.write(data));

    std::size_t num_calls{};
    buf.consume([&](std::span<float const> const d) {
        switch (num_calls)
        {
            case 0:
                EXPECT_TRUE(std::ranges::equal(d, std::array{0.f, 1.f, 2.f}));
                break;

            case 1:
                EXPECT_TRUE(std::ranges::equal(d, std::array{3.f}));
                break;
        }

        ++num_calls;
    });

    EXPECT_EQ(2u, num_calls);
}

TEST(audio_spsc_ring_buffer,
     write_to_full_capacity_consume_on_border_will_call_the_functor_twice)
{
    audio_spsc_ring_buffer buf(6);

    ASSERT_EQ(4u, buf.write(std::array{0.f, 0.f, 0.f, 0.f}));
    buf.consume([](auto const&) {});

    std::array data{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f};

    ASSERT_EQ(6u, buf.write(data));

    std::size_t num_calls{};
    buf.consume([&](std::span<float const> const d) {
        switch (num_calls)
        {
            case 0:
                EXPECT_TRUE(std::ranges::equal(d, std::array{0.f, 1.f, 2.f}));
                break;

            case 1:
                EXPECT_TRUE(std::ranges::equal(d, std::array{3.f, 4.f, 5.f}));
                break;
        }

        ++num_calls;
    });

    EXPECT_EQ(2u, num_calls);
}

} // namespace piejam::audio::engine::test
