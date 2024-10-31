// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/midi_learn_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/slice.h>
#include <piejam/midi/event.h>

#include <gtest/gtest.h>

namespace piejam::runtime::processors::test
{

struct midi_learn_processor_test : testing::Test
{
    midi_learn_processor_test()
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
    audio::engine::event_buffer<midi::external_event> ev_in_buf{ev_buf_pmr_mem};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    audio::engine::process_context ctx{
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = 16};

    std::unique_ptr<audio::engine::processor> proc{make_midi_learn_processor()};
};

TEST_F(midi_learn_processor_test, with_empty_input)
{
    proc->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<midi::external_event>(0).empty());
    EXPECT_TRUE(ev_out_bufs.get<midi::external_event>(1).empty());
}

TEST_F(midi_learn_processor_test, with_event)
{
    midi::external_event ev{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 1,
                    .data = midi::cc_event{.cc = 5, .value = 23}}};

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    {
        ASSERT_EQ(1u, ev_out_bufs.get<midi::external_event>(0).size());
        auto const& evr = *ev_out_bufs.get<midi::external_event>(0).begin();
        EXPECT_EQ(1u, evr.offset());
        EXPECT_EQ(ev, evr.value());
    }

    {
        ASSERT_EQ(1u, ev_out_bufs.get<midi::external_event>(1).size());
        auto const& evr = *ev_out_bufs.get<midi::external_event>(1).begin();
        EXPECT_EQ(1u, evr.offset());
        EXPECT_EQ(ev, evr.value());
    }
}

TEST_F(midi_learn_processor_test, with_multiple_event)
{
    midi::external_event ev1{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 1,
                    .data = midi::cc_event{.cc = 5, .value = 23}}};

    midi::external_event ev2{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 2,
                    .data = midi::cc_event{.cc = 7, .value = 58}}};

    ev_in_buf.insert(1u, ev1);
    ev_in_buf.insert(3u, ev2);
    proc->process(ctx);

    {
        ASSERT_EQ(1u, ev_out_bufs.get<midi::external_event>(0).size());
        auto const& evr1 = *ev_out_bufs.get<midi::external_event>(0).begin();
        EXPECT_EQ(1u, evr1.offset());
        EXPECT_EQ(ev1, evr1.value());
    }

    {
        ASSERT_EQ(2u, ev_out_bufs.get<midi::external_event>(1).size());
        auto const& evr1 = *ev_out_bufs.get<midi::external_event>(1).begin();
        EXPECT_EQ(1u, evr1.offset());
        EXPECT_EQ(ev1, evr1.value());

        auto const& evr2 =
                *std::next(ev_out_bufs.get<midi::external_event>(1).begin());
        EXPECT_EQ(3u, evr2.offset());
        EXPECT_EQ(ev2, evr2.value());
    }
}

TEST_F(midi_learn_processor_test, with_single_event_on_multiple_process)
{
    midi::external_event ev1{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 1,
                    .data = midi::cc_event{.cc = 5, .value = 23}}};

    midi::external_event ev2{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 2,
                    .data = midi::cc_event{.cc = 7, .value = 58}}};

    ev_in_buf.insert(1u, ev1);
    proc->process(ctx);

    ev_in_buf.clear();
    ev_in_buf.insert(3u, ev2);
    proc->process(ctx);

    {
        ASSERT_EQ(1u, ev_out_bufs.get<midi::external_event>(0).size());
        auto const& evr1 = *ev_out_bufs.get<midi::external_event>(0).begin();
        EXPECT_EQ(1u, evr1.offset());
        EXPECT_EQ(ev1, evr1.value());
    }

    {
        ASSERT_EQ(2u, ev_out_bufs.get<midi::external_event>(1).size());
        auto const& evr1 = *ev_out_bufs.get<midi::external_event>(1).begin();
        EXPECT_EQ(1u, evr1.offset());
        EXPECT_EQ(ev1, evr1.value());

        auto const& evr2 =
                *std::next(ev_out_bufs.get<midi::external_event>(1).begin());
        EXPECT_EQ(3u, evr2.offset());
        EXPECT_EQ(ev2, evr2.value());
    }
}

} // namespace piejam::runtime::processors::test
