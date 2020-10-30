// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "processor_mock.h"

#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/thread_context.h>
#include <piejam/container/table.h>

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
    auto d = graph_to_dag(g, 8, buffer_size);

    EXPECT_CALL(in_proc, process(_)).WillOnce(Invoke([](auto const& ctx) {
        ASSERT_EQ(1u, ctx.outputs.size());
        auto const& buf = ctx.outputs[0];
        ASSERT_EQ(1u, buf.size());
        buf[0] = 23.f;
    }));

    auto input_has_sample = [](process_context const& ctx) {
        return ctx.inputs.size() == 1 && ctx.inputs[0].get().size() == 1 &&
               ctx.inputs[0].get()[0] == 23.f;
    };

    EXPECT_CALL(out_proc, process(Truly(input_has_sample))).Times(1);

    d({});
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
    auto d = graph_to_dag(g, 8, buffer_size);

    EXPECT_CALL(in_proc, process(_)).WillOnce(Invoke([](auto const& ctx) {
        ASSERT_EQ(1u, ctx.outputs.size());
        auto const& buf = ctx.outputs[0];
        ASSERT_EQ(1u, buf.size());
        buf[0] = 23.f;
    }));

    auto input_has_sample = [](process_context const& ctx) {
        return ctx.inputs.size() == 2 && ctx.inputs[0].get().size() == 1 &&
               ctx.inputs[0].get()[0] == 23.f &&
               ctx.inputs[1].get().size() == 1 &&
               ctx.inputs[1].get()[0] == 23.f;
    };

    EXPECT_CALL(out_proc, process(Truly(input_has_sample))).Times(1);

    d({});
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
    auto d = graph_to_dag(g, 8, buffer_size);

    EXPECT_CALL(in_proc, process(_)).Times(1);
    EXPECT_CALL(out_proc, process(_)).Times(1);

    d({});
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
    auto d = graph_to_dag(g, 8, buffer_size);

    EXPECT_CALL(in_proc, process(_)).Times(1);

    auto valid_ins = [](process_context const& ctx) {
        return ctx.inputs.size() == 2 && ctx.inputs[0].get().size() == 1 &&
               ctx.inputs[1].get().empty();
    };

    EXPECT_CALL(out_proc, process(Truly(valid_ins))).Times(1);

    d({});
}

TEST(graph_to_dag, event_is_transferred)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    std::array event_in_ports{event_port{typeid(float), {}}};
    std::array event_out_ports{event_port{typeid(float), {}}};

    ON_CALL(in_proc, event_outputs()).WillByDefault(Return(event_in_ports));
    ON_CALL(in_proc, create_event_output_buffers(_))
            .WillByDefault(Invoke([](event_output_buffer_factory const& fac) {
                fac.add<float>();
            }));
    ON_CALL(out_proc, event_inputs()).WillByDefault(Return(event_out_ports));

    g.add_event_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g, 8, buffer_size);

    EXPECT_CALL(in_proc, process(_))
            .WillOnce(Invoke([](process_context const& ctx) {
                auto& ev_buf = ctx.event_outputs.template get<float>(0);
                ev_buf.insert(5, 23.f);
                EXPECT_EQ(1u, ev_buf.size());
            }));

    auto event_input_has_event = [](process_context const& ctx) {
        auto* in_buf = ctx.event_inputs.size() == 1
                               ? ctx.event_inputs.get<float>(0)
                               : nullptr;
        return in_buf && in_buf->size() == 1 &&
               in_buf->begin()->offset() == 5 &&
               in_buf->begin()->value() == 23.f;
    };
    EXPECT_CALL(out_proc, process(Truly(event_input_has_event))).Times(1);

    d({});
}

TEST(graph_to_dag, event_output_buffer_is_cleared_after_dag_run)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    std::array event_out_ports{event_port{typeid(float), {}}};

    ON_CALL(in_proc, num_outputs()).WillByDefault(Return(1));
    ON_CALL(in_proc, event_outputs()).WillByDefault(Return(event_out_ports));
    ON_CALL(in_proc, create_event_output_buffers(_))
            .WillByDefault(Invoke([](event_output_buffer_factory const& fac) {
                fac.add<float>();
            }));
    ON_CALL(out_proc, num_inputs()).WillByDefault(Return(1));
    g.add_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g, 8, buffer_size);

    event_buffer<float>* ev_buf{};
    EXPECT_CALL(in_proc, process(_))
            .WillOnce(Invoke([&ev_buf](process_context const& ctx) {
                ev_buf = &ctx.event_outputs.template get<float>(0);
                ev_buf->insert(5, 23.f);
                EXPECT_EQ(1u, ev_buf->size());
            }));

    d({});

    ASSERT_NE(nullptr, ev_buf);
    EXPECT_TRUE(ev_buf->empty());
}

} // namespace piejam::audio::engine::test
