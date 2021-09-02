// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/midi_input_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/midi/event.h>
#include <piejam/midi/event_handler.h>
#include <piejam/midi/input_event_handler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace piejam::runtime::processors::test
{

struct midi_input_event_handler_mock : public midi::input_event_handler
{
    MOCK_METHOD(void, process, (midi::event_handler&));
};

struct midi_input_processor_test : testing::Test
{
    midi_input_processor_test()
    {
        auto mock = std::make_unique<
                testing::StrictMock<midi_input_event_handler_mock>>();
        in_ev_handler = mock.get();
        proc = make_midi_input_processor(std::move(mock));

        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());

        for (auto const& ev_port : proc->event_outputs())
            ev_out_bufs.add(ev_port);
    }

    audio::engine::event_buffer_memory ev_buf_mem{1024};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    audio::engine::process_context ctx{
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = 16};

    midi_input_event_handler_mock* in_ev_handler{};
    std::unique_ptr<audio::engine::processor> proc;
};

TEST_F(midi_input_processor_test, with_empty_input)
{
    using testing::_;
    EXPECT_CALL(*in_ev_handler, process(_));
    proc->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<midi::external_event>(0).empty());
}

TEST_F(midi_input_processor_test, with_event)
{
    midi::external_event ev{
            .device_id = midi::device_id_t::generate(),
            .event = midi::channel_cc_event{
                    .channel = 1,
                    .data = midi::cc_event{.cc = 5, .value = 23}}};

    using testing::_;
    EXPECT_CALL(*in_ev_handler, process(_)).WillOnce([&ev](auto& handler) {
        handler.process(ev);
    });
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.get<midi::external_event>(0).size());
    auto const& evr = *ev_out_bufs.get<midi::external_event>(0).begin();
    EXPECT_EQ(0u, evr.offset());
    EXPECT_EQ(ev, evr.value());
}

TEST_F(midi_input_processor_test, with_multiple_event)
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

    using testing::_;
    EXPECT_CALL(*in_ev_handler, process(_)).WillOnce([&](auto& handler) {
        handler.process(ev1);
        handler.process(ev2);
    });
    proc->process(ctx);

    ASSERT_EQ(2u, ev_out_bufs.get<midi::external_event>(0).size());
    auto const& evr1 = *ev_out_bufs.get<midi::external_event>(0).begin();
    EXPECT_EQ(0u, evr1.offset());
    EXPECT_EQ(ev1, evr1.value());

    auto const& evr2 =
            *std::next(ev_out_bufs.get<midi::external_event>(0).begin());
    EXPECT_EQ(0u, evr2.offset());
    EXPECT_EQ(ev2, evr2.value());
}

} // namespace piejam::runtime::processors::test
