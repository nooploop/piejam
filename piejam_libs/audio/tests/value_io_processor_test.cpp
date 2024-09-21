// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/value_io_processor.h>

#include <piejam/audio/engine/processor_test_environment.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

TEST(value_io_processor_test,
     initial_process_will_not_send_anything_if_nothing_is_set)
{
    value_io_processor<float> sut;
    processor_test_environment test_env(sut, 16);

    sut.process(test_env.ctx);

    auto const& ev_out_buf = test_env.event_outputs.get<float>(0);
    EXPECT_TRUE(ev_out_buf.empty());
}

TEST(value_io_processor_test, send_event_to_out_if_value_is_set)
{
    value_io_processor<float> sut;
    processor_test_environment test_env(sut, 16);

    sut.set(0.23f);
    sut.process(test_env.ctx);

    auto const& ev_out_buf = test_env.event_outputs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    EXPECT_EQ(0u, ev_out_buf.begin()->offset());
    EXPECT_EQ(0.23f, ev_out_buf.begin()->value());
}

TEST(value_io_processor_test, no_event_in_second_process_after_first_set)
{
    value_io_processor<float> sut;
    processor_test_environment test_env(sut, 16);

    sut.set(0.23f);
    sut.process(test_env.ctx);

    auto const& ev_out_buf = test_env.event_outputs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    test_env.event_outputs.clear_buffers();

    sut.process(test_env.ctx);
    EXPECT_TRUE(ev_out_buf.empty());
}

TEST(value_io_processor_test,
     for_multiple_set_value_generate_output_event_with_last_value)
{
    value_io_processor<float> sut;
    processor_test_environment test_env(sut, 16);

    sut.set(0.23f);
    sut.set(0.58f);
    sut.process(test_env.ctx);

    auto const& ev_out_buf = test_env.event_outputs.get<float>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    EXPECT_EQ(0u, ev_out_buf.begin()->offset());
    EXPECT_EQ(0.58f, ev_out_buf.begin()->value());
}

TEST(value_io_processor_test, no_value_initially)
{
    value_io_processor<int> sut;
    sut.consume([](int) { FAIL(); });
}

TEST(value_io_processor_test, no_value_after_process_with_no_events)
{
    value_io_processor<int> sut;
    processor_test_environment test_env(sut, 16);
    sut.process(test_env.ctx);
    sut.consume([](int) { FAIL(); });
}

TEST(value_io_processor_test, value_on_event)
{
    value_io_processor<int> sut;
    processor_test_environment test_env(sut, 16);
    test_env.insert_input_event<int>(0, 5, 23);
    sut.process(test_env.ctx);

    int result{};
    EXPECT_TRUE(sut.get(result));
    EXPECT_EQ(23, result);
}

TEST(value_io_processor_test, output_event_on_input_event)
{
    value_io_processor<int> sut;
    processor_test_environment test_env(sut, 16);
    test_env.insert_input_event<int>(0, 5, 23);
    sut.process(test_env.ctx);

    auto const& ev_out_buf = test_env.event_outputs.get<int>(0);
    ASSERT_EQ(1u, ev_out_buf.size());
    EXPECT_EQ(5u, ev_out_buf.begin()->offset());
    EXPECT_EQ(23, ev_out_buf.begin()->value());
}

} // namespace piejam::audio::engine::test
