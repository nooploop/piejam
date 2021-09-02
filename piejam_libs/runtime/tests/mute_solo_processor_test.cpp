// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/mute_solo_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>

#include <gtest/gtest.h>

namespace piejam::runtime::processors::test
{

struct mute_solo_processor_test
    : testing::TestWithParam<std::tuple<bool, bool, float>>
{
    mute_solo_processor_test()
    {
        ev_out_bufs.set_event_memory(&ev_buf_mem.memory_resource());
        for (auto const& ev_port : proc->event_outputs())
            ev_out_bufs.add(ev_port);

        for (auto const& ev_port : proc->event_inputs())
            ev_in_bufs.add(ev_port);

        ev_in_bufs.set(0, ev_in_mute_buf);
        ev_in_bufs.set(1, ev_in_muted_by_solo_buf);
    }

    audio::engine::event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_buf_pmr_mem{&ev_buf_mem.memory_resource()};
    audio::engine::event_buffer<bool> ev_in_mute_buf{ev_buf_pmr_mem};
    audio::engine::event_buffer<bool> ev_in_muted_by_solo_buf{ev_buf_pmr_mem};
    audio::engine::event_input_buffers ev_in_bufs;
    audio::engine::event_output_buffers ev_out_bufs;
    audio::engine::process_context ctx{
            .event_inputs = ev_in_bufs,
            .event_outputs = ev_out_bufs,
            .buffer_size = 16};

    std::unique_ptr<audio::engine::processor> proc{make_mute_solo_processor()};
};

TEST_F(mute_solo_processor_test, no_events)
{
    proc->process(ctx);

    EXPECT_TRUE(ev_out_bufs.get<float>(0).empty());
}

TEST_P(mute_solo_processor_test, mute_events_at_same_offset)
{
    ev_in_mute_buf.insert(0u, std::get<0>(GetParam()));
    ev_in_muted_by_solo_buf.insert(0u, std::get<1>(GetParam()));
    proc->process(ctx);

    ASSERT_EQ(1u, ev_out_bufs.get<float>(0).size());
    auto const& evr = *ev_out_bufs.get<float>(0).begin();
    EXPECT_EQ(0u, evr.offset());
    EXPECT_FLOAT_EQ(std::get<2>(GetParam()), evr.value());
}

INSTANTIATE_TEST_SUITE_P(
        values,
        mute_solo_processor_test,
        testing::Values(
                std::make_tuple(false, false, 1.f),
                std::make_tuple(true, false, 0.f),
                std::make_tuple(false, true, 0.f),
                std::make_tuple(true, true, 0.f)));

TEST_F(mute_solo_processor_test, multiple_events)
{
    ev_in_mute_buf.insert(0u, false);
    ev_in_muted_by_solo_buf.insert(1u, true);
    ev_in_muted_by_solo_buf.insert(5u, false);
    proc->process(ctx);

    ASSERT_EQ(3u, ev_out_bufs.get<float>(0).size());

    auto const& evr1 = *ev_out_bufs.get<float>(0).begin();
    EXPECT_EQ(0u, evr1.offset());
    EXPECT_FLOAT_EQ(1.f, evr1.value());

    auto const& evr2 = *std::next(ev_out_bufs.get<float>(0).begin());
    EXPECT_EQ(1u, evr2.offset());
    EXPECT_FLOAT_EQ(0.f, evr2.value());

    auto const& evr3 = *std::next(ev_out_bufs.get<float>(0).begin(), 2);
    EXPECT_EQ(5u, evr3.offset());
    EXPECT_FLOAT_EQ(1.f, evr3.value());
}

} // namespace piejam::runtime::processors::test
