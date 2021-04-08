// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "processor_mock.h"

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/dag_executor.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/thread_context.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(graph_to_dag, audio_is_transferred_to_connected_proc)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    ON_CALL(in_proc, num_outputs()).WillByDefault(Return(1));
    ON_CALL(out_proc, num_inputs()).WillByDefault(Return(1));
    g.add_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g).make_runnable();

    EXPECT_CALL(in_proc, process(_))
            .WillOnce(Invoke([](process_context const& ctx) {
                ASSERT_EQ(1u, ctx.outputs.size());
                auto const& buf = ctx.outputs[0];
                ASSERT_EQ(1u, buf.size());
                buf[0] = 23.f;
                ctx.results[0] = buf;
            }));

    auto input_has_sample = [](process_context const& ctx) {
        return ctx.inputs.size() == 1 &&
               ctx.inputs[0].get().buffer().size() == 1 &&
               ctx.inputs[0].get().buffer()[0] == 23.f;
    };

    EXPECT_CALL(out_proc, process(Truly(input_has_sample))).Times(1);

    (*d)(buffer_size);
}

TEST(graph_to_dag, audio_can_spread_to_multiple_ins)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    ON_CALL(in_proc, num_outputs()).WillByDefault(Return(1));
    ON_CALL(out_proc, num_inputs()).WillByDefault(Return(2));
    g.add_wire({in_proc, 0}, {out_proc, 0});
    g.add_wire({in_proc, 0}, {out_proc, 1});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g).make_runnable();

    EXPECT_CALL(in_proc, process(_))
            .WillOnce(Invoke([](process_context const& ctx) {
                ASSERT_EQ(1u, ctx.outputs.size());
                auto const& buf = ctx.outputs[0];
                ASSERT_EQ(1u, buf.size());
                buf[0] = 23.f;
                ctx.results[0] = buf;
            }));

    auto input_has_sample = [](process_context const& ctx) {
        return ctx.inputs.size() == 2 &&
               ctx.inputs[0].get().buffer().size() == 1 &&
               ctx.inputs[0].get().buffer()[0] == 23.f &&
               ctx.inputs[1].get().buffer().size() == 1 &&
               ctx.inputs[1].get().buffer()[0] == 23.f;
    };

    EXPECT_CALL(out_proc, process(Truly(input_has_sample))).Times(1);

    (*d)(buffer_size);
}

TEST(graph_to_dag, out_processor_is_called_once_even_with_two_wires_from_in)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    ON_CALL(in_proc, num_outputs()).WillByDefault(Return(2));
    ON_CALL(out_proc, num_inputs()).WillByDefault(Return(2));
    g.add_wire({in_proc, 0}, {out_proc, 0});
    g.add_wire({in_proc, 1}, {out_proc, 1});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g).make_runnable();

    EXPECT_CALL(in_proc, process(_)).Times(1);
    EXPECT_CALL(out_proc, process(_)).Times(1);

    (*d)(buffer_size);
}

TEST(graph_to_dag, unconnected_input_will_have_silence_buffer)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    ON_CALL(in_proc, num_outputs()).WillByDefault(Return(1));
    ON_CALL(out_proc, num_inputs()).WillByDefault(Return(2));
    g.add_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g).make_runnable();

    EXPECT_CALL(in_proc, process(_))
            .WillOnce(Invoke([](process_context const& ctx) {
                ASSERT_EQ(1u, ctx.outputs.size());
                ASSERT_EQ(1u, ctx.results.size());
                ctx.results[0] = ctx.outputs[0];
            }));

    auto valid_ins = [](process_context const& ctx) {
        return ctx.inputs.size() == 2 &&
               ctx.inputs[0].get().buffer().size() == 1 &&
               is_silence(ctx.inputs[1]);
    };

    EXPECT_CALL(out_proc, process(Truly(valid_ins))).Times(1);

    (*d)(buffer_size);
}

TEST(graph_to_dag, event_is_transferred)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    std::array event_in_ports{event_port(std::in_place_type<float>, {})};
    std::array event_out_ports{event_port(std::in_place_type<float>, {})};

    ON_CALL(in_proc, event_outputs()).WillByDefault(Return(event_in_ports));
    ON_CALL(out_proc, event_inputs()).WillByDefault(Return(event_out_ports));

    g.add_event_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g).make_runnable();

    EXPECT_CALL(in_proc, process(_))
            .WillOnce(Invoke([](process_context const& ctx) {
                auto& ev_buf = ctx.event_outputs.template get<float>(0);
                ev_buf.insert(5, 23.f);
                EXPECT_EQ(1u, ev_buf.size());
            }));

    auto event_input_has_event = [](process_context const& ctx) {
        auto const& in_buf = ctx.event_inputs.get<float>(0);
        return in_buf.size() == 1 && in_buf.begin()->offset() == 5 &&
               in_buf.begin()->value() == 23.f;
    };
    EXPECT_CALL(out_proc, process(Truly(event_input_has_event))).Times(1);

    (*d)(buffer_size);
}

TEST(graph_to_dag, event_output_buffer_is_cleared_after_dag_run)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    std::array event_out_ports{event_port(std::in_place_type<float>, {})};

    ON_CALL(in_proc, num_outputs()).WillByDefault(Return(1));
    ON_CALL(in_proc, event_outputs()).WillByDefault(Return(event_out_ports));
    ON_CALL(out_proc, num_inputs()).WillByDefault(Return(1));
    g.add_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g).make_runnable();

    event_buffer<float>* ev_buf{};
    EXPECT_CALL(in_proc, process(_))
            .WillOnce(Invoke([&ev_buf](process_context const& ctx) {
                ev_buf = &ctx.event_outputs.template get<float>(0);
                ev_buf->insert(5, 23.f);
                EXPECT_EQ(1u, ev_buf->size());
            }));

    (*d)(buffer_size);

    ASSERT_NE(nullptr, ev_buf);
    EXPECT_TRUE(ev_buf->empty());
}

} // namespace piejam::audio::engine::test
