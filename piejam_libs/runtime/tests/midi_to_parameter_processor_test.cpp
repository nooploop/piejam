// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/midi_to_parameter_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/midi/event.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>

#include <gtest/gtest.h>

namespace piejam::runtime::processors::test
{

struct midi_to_bool_parameter_processor_test : testing::Test
{
    midi_to_bool_parameter_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
        for (auto const& ev_port : proc->event_outputs())
        {
            ev_out_bufs.add(ev_port);
        }

        for (auto const& ev_port : proc->event_inputs())
        {
            ev_in_bufs.add(ev_port);
        }

        ev_in_bufs.set(0, ev_in_buf);
    }

    audio::engine::event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_buf_pmr_mem{&ev_buf_mem.memory_resource()};
    audio::engine::event_buffer<midi::cc_event> ev_in_buf{ev_buf_pmr_mem};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    audio::engine::process_context ctx{
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = 16};

    bool_parameter param;

    std::unique_ptr<audio::engine::processor> proc{
            make_midi_cc_to_parameter_processor(param)};
};

TEST_F(midi_to_bool_parameter_processor_test, with_empty_input)
{
    proc->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<bool>(0).empty());
}

TEST_F(midi_to_bool_parameter_processor_test, with_event_to_false)
{
    midi::cc_event ev{.cc = 5, .value = 0};

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.get<bool>(0).size());
    auto const& evr = *ev_out_bufs.get<bool>(0).begin();
    EXPECT_EQ(1u, evr.offset());
    EXPECT_EQ(false, evr.value());
}

TEST_F(midi_to_bool_parameter_processor_test, with_event_to_true)
{
    midi::cc_event ev{.cc = 5, .value = 23}; // some value not != 0

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.get<bool>(0).size());
    auto const& evr = *ev_out_bufs.get<bool>(0).begin();
    EXPECT_EQ(1u, evr.offset());
    EXPECT_EQ(true, evr.value());
}

struct midi_to_float_parameter_processor_test
    : testing::TestWithParam<std::pair<std::size_t, float>>
{
    midi_to_float_parameter_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
        for (auto const& ev_port : proc->event_outputs())
        {
            ev_out_bufs.add(ev_port);
        }

        for (auto const& ev_port : proc->event_inputs())
        {
            ev_in_bufs.add(ev_port);
        }

        ev_in_bufs.set(0, ev_in_buf);
    }

    audio::engine::event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_buf_pmr_mem{&ev_buf_mem.memory_resource()};
    audio::engine::event_buffer<midi::cc_event> ev_in_buf{ev_buf_pmr_mem};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    audio::engine::process_context ctx{
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = 16};

    float_parameter param{
            .default_value = 7.f,
            .min = 5.f,
            .max = 10.f,
            .to_normalized = &parameter::to_normalized_linear,
            .from_normalized = &parameter::from_normalized_linear};

    std::unique_ptr<audio::engine::processor> proc{
            make_midi_cc_to_parameter_processor(param)};
};

TEST_F(midi_to_float_parameter_processor_test, with_empty_input)
{
    proc->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<float>(0).empty());
}

TEST_P(midi_to_float_parameter_processor_test, with_event_cc_value)
{
    midi::cc_event ev{.cc = 5, .value = GetParam().first};

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.get<float>(0).size());
    auto const& evr = *ev_out_bufs.get<float>(0).begin();
    EXPECT_EQ(1u, evr.offset());
    EXPECT_FLOAT_EQ(GetParam().second, evr.value());
}

INSTANTIATE_TEST_SUITE_P(
        values,
        midi_to_float_parameter_processor_test,
        testing::Values(
                std::make_pair(0u, 5.f),
                std::make_pair(64u, 5.f * (64.f / 127.f) + 5.f),
                std::make_pair(127u, 10.f)));

struct midi_to_int_parameter_processor_test
    : testing::TestWithParam<std::pair<std::size_t, int>>
{
    midi_to_int_parameter_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
        for (auto const& ev_port : proc->event_outputs())
        {
            ev_out_bufs.add(ev_port);
        }

        for (auto const& ev_port : proc->event_inputs())
        {
            ev_in_bufs.add(ev_port);
        }

        ev_in_bufs.set(0, ev_in_buf);
    }

    audio::engine::event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_buf_pmr_mem{&ev_buf_mem.memory_resource()};
    audio::engine::event_buffer<midi::cc_event> ev_in_buf{ev_buf_pmr_mem};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    audio::engine::process_context ctx{
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = 16};

    int_parameter param{.default_value = 10, .min = 10, .max = 19};

    std::unique_ptr<audio::engine::processor> proc{
            make_midi_cc_to_parameter_processor(param)};
};

TEST_F(midi_to_int_parameter_processor_test, with_empty_input)
{
    proc->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<int>(0).empty());
}

TEST_P(midi_to_int_parameter_processor_test, with_event_cc_value)
{
    midi::cc_event ev{.cc = 5, .value = GetParam().first};

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.get<int>(0).size());
    auto const& evr = *ev_out_bufs.get<int>(0).begin();
    EXPECT_EQ(1u, evr.offset());
    EXPECT_EQ(GetParam().second, evr.value());
}

INSTANTIATE_TEST_SUITE_P(
        values,
        midi_to_int_parameter_processor_test,
        testing::Values(
                std::make_pair(0u, 10),
                std::make_pair(5u, 15),
                std::make_pair(9u, 19),
                std::make_pair(10u, 19),
                std::make_pair(20u, 19),
                std::make_pair(127u, 19)));

} // namespace piejam::runtime::processors::test
