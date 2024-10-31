// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/midi_assignment_processor.h>

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

struct midi_assignment_processor_test : testing::Test
{
    midi_assignment_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
    }

    void setup(midi_assignments_map const& assigns)
    {
        proc = make_midi_assignment_processor(assigns);

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

    std::unique_ptr<audio::engine::processor> proc;
};

TEST_F(midi_assignment_processor_test, with_empty_input)
{
    setup({});
    EXPECT_TRUE(ev_out_bufs.empty());

    proc->process(ctx);
}

TEST_F(midi_assignment_processor_test, with_mapped_event)
{
    setup({{midi_assignment_id{float_parameter_id::generate()},
            midi_assignment{
                    .channel = 1,
                    .control_type = midi_assignment::type::cc,
                    .control_id = 5}}});

    midi::external_event ev{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 1,
                    .data = midi::cc_event{.cc = 5, .value = 23}}};

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.size());

    ASSERT_EQ(1u, ev_out_bufs.get<midi::cc_event>(0).size());
    auto const& evr = *ev_out_bufs.get<midi::cc_event>(0).begin();
    EXPECT_EQ(1u, evr.offset());
    EXPECT_EQ(std::get<midi::channel_cc_event>(ev.event).data, evr.value());
}

TEST_F(midi_assignment_processor_test, with_event_not_matching_cc)
{
    setup({{midi_assignment_id{float_parameter_id::generate()},
            midi_assignment{
                    .channel = 1,
                    .control_type = midi_assignment::type::cc,
                    .control_id = 5}}});

    midi::external_event ev{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 1,
                    .data = midi::cc_event{.cc = 3, .value = 23}}};

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.size());

    EXPECT_TRUE(ev_out_bufs.get<midi::cc_event>(0).empty());
}

TEST_F(midi_assignment_processor_test, with_event_not_matching_channel)
{
    setup({{midi_assignment_id{float_parameter_id::generate()},
            midi_assignment{
                    .channel = 1,
                    .control_type = midi_assignment::type::cc,
                    .control_id = 5}}});

    midi::external_event ev{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 2,
                    .data = midi::cc_event{.cc = 5, .value = 23}}};

    ev_in_buf.insert(1u, ev);
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.size());

    EXPECT_TRUE(ev_out_bufs.get<midi::cc_event>(0).empty());
}

TEST_F(midi_assignment_processor_test, with_multiple_assignments)
{
    setup({{midi_assignment_id{float_parameter_id::generate()},
            midi_assignment{
                    .channel = 1,
                    .control_type = midi_assignment::type::cc,
                    .control_id = 5}},
           {midi_assignment_id{float_parameter_id::generate()},
            midi_assignment{
                    .channel = 2,
                    .control_type = midi_assignment::type::cc,
                    .control_id = 7}}});

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

    ASSERT_EQ(2u, ev_out_bufs.size());

    {
        ASSERT_EQ(1u, ev_out_bufs.get<midi::cc_event>(0).size());
        auto const& evr = *ev_out_bufs.get<midi::cc_event>(0).begin();
        EXPECT_EQ(1u, evr.offset());
        EXPECT_EQ(
                std::get<midi::channel_cc_event>(ev1.event).data,
                evr.value());
    }

    {
        ASSERT_EQ(1u, ev_out_bufs.get<midi::cc_event>(1).size());
        auto const& evr = *ev_out_bufs.get<midi::cc_event>(1).begin();
        EXPECT_EQ(3u, evr.offset());
        EXPECT_EQ(
                std::get<midi::channel_cc_event>(ev2.event).data,
                evr.value());
    }
}

} // namespace piejam::runtime::processors::test
