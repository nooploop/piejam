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
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_to_dag.h>
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

    EXPECT_CALL(in_proc, process(_, _, _, _, _))
            .WillOnce(Invoke([](auto const&,
                                auto const& outs,
                                auto const&,
                                auto const&,
                                auto const&) {
                ASSERT_EQ(1u, outs.size());
                auto const& buf = outs[0];
                ASSERT_EQ(1u, buf.size());
                buf[0] = 23.f;
            }));

    auto has_sample = [](processor::input_buffers_t const& bufs) {
        return bufs.size() == 1 && bufs[0].get().size() == 1 &&
               bufs[0].get()[0] == 23.f;
    };

    EXPECT_CALL(out_proc, process(Truly(has_sample), _, _, _, _)).Times(1);

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

    EXPECT_CALL(in_proc, process(_, _, _, _, _))
            .WillOnce(Invoke([](auto const&,
                                auto const& outs,
                                auto const&,
                                auto const&,
                                auto const&) {
                ASSERT_EQ(1u, outs.size());
                auto const& buf = outs[0];
                ASSERT_EQ(1u, buf.size());
                buf[0] = 23.f;
            }));

    auto has_sample = [](processor::input_buffers_t const& bufs) {
        return bufs.size() == 2 && bufs[0].get().size() == 1 &&
               bufs[0].get()[0] == 23.f && bufs[1].get().size() == 1 &&
               bufs[1].get()[0] == 23.f;
    };

    EXPECT_CALL(out_proc, process(Truly(has_sample), _, _, _, _)).Times(1);

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

    EXPECT_CALL(in_proc, process(_, _, _, _, _)).Times(1);
    EXPECT_CALL(out_proc, process(_, _, _, _, _)).Times(1);

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

    EXPECT_CALL(in_proc, process(_, _, _, _, _)).Times(1);

    auto valid_ins = [](processor::input_buffers_t const& ins) {
        return ins.size() == 2 && ins[0].get().size() == 1 &&
               ins[1].get().empty();
    };

    EXPECT_CALL(out_proc, process(Truly(valid_ins), _, _, _, _)).Times(1);

    d({});
}

TEST(graph_to_dag, event_is_transferred)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    ON_CALL(in_proc, num_event_outputs()).WillByDefault(Return(1));
    ON_CALL(in_proc, create_event_output_buffers(_))
            .WillByDefault(Invoke([](event_output_buffer_factory const& fac) {
                fac.add<float>();
            }));
    ON_CALL(out_proc, num_event_inputs()).WillByDefault(Return(1));

    g.add_event_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g, 8, buffer_size);

    EXPECT_CALL(in_proc, process(_, _, _, _, _))
            .WillOnce(Invoke([](auto const&,
                                auto const&,
                                auto const&,
                                auto const&,
                                auto const& ev_outs) {
                auto& ev_buf = ev_outs.template get<float>(0);
                ev_buf.insert(5, 23.f);
                EXPECT_EQ(1u, ev_buf.size());
            }));

    auto has_event = [](event_input_buffers const& in_bufs) {
        auto* in_buf = in_bufs.size() == 1 ? in_bufs.get<float>(0) : nullptr;
        return in_buf && in_buf->size() == 1 &&
               in_buf->begin()->offset() == 5 &&
               in_buf->begin()->value() == 23.f;
    };
    EXPECT_CALL(out_proc, process(_, _, _, Truly(has_event), _)).Times(1);

    d({});
}

TEST(graph_to_dag, event_output_buffer_is_cleared_after_dag_run)
{
    ::testing::NiceMock<processor_mock> in_proc;
    ::testing::NiceMock<processor_mock> out_proc;

    graph g;

    using namespace testing;

    ON_CALL(in_proc, num_outputs()).WillByDefault(Return(1));
    ON_CALL(in_proc, num_event_outputs()).WillByDefault(Return(1));
    ON_CALL(in_proc, create_event_output_buffers(_))
            .WillByDefault(Invoke([](event_output_buffer_factory const& fac) {
                fac.add<float>();
            }));
    ON_CALL(out_proc, num_inputs()).WillByDefault(Return(1));
    g.add_wire({in_proc, 0}, {out_proc, 0});

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g, 8, buffer_size);

    event_buffer<float>* ev_buf{};
    EXPECT_CALL(in_proc, process(_, _, _, _, _))
            .WillOnce(Invoke([&ev_buf](
                                     auto const&,
                                     auto const&,
                                     auto const&,
                                     auto const&,
                                     auto const& ev_outs) {
                ev_buf = &ev_outs.template get<float>(0);
                ev_buf->insert(5, 23.f);
                EXPECT_EQ(1u, ev_buf->size());
            }));

    d({});

    ASSERT_NE(nullptr, ev_buf);
    EXPECT_TRUE(ev_buf->empty());
}

} // namespace piejam::audio::engine::test
