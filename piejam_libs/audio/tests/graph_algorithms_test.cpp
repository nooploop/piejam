// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/graph_algorithms.h>

#include "component_mock.h"
#include "fake_processor.h"
#include "processor_mock.h"

#include <piejam/audio/engine/event_identity_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/slice.h>

#include <gtest/gtest.h>

#include <fstream>

namespace piejam::audio::engine::test
{

TEST(finalize_graph, connect_to_already_connected_will_insert_a_mixer)
{
    using namespace testing;

    fake_processor src1{"src1", 0, 1};
    fake_processor src2{"src2", 0, 1};
    fake_processor dst{"dst", 1, 0};
    graph sut;
    sut.audio.insert({src1, 0}, {dst, 0});
    EXPECT_TRUE(has_audio_wire(sut, {src1, 0}, {dst, 0}));
    sut.audio.insert({src2, 0}, {dst, 0});

    auto [result, mixers] = finalize_graph(sut);

    EXPECT_FALSE(has_audio_wire(result, {src1, 0}, {dst, 0}));
    EXPECT_FALSE(has_audio_wire(result, {src2, 0}, {dst, 0}));
    ASSERT_EQ(1u, mixers.size());
    EXPECT_TRUE(is_mix_processor(*mixers.front()));
    EXPECT_EQ(2u, mixers.front()->num_inputs());
    EXPECT_TRUE(has_audio_wire(result, {src1, 0}, {*mixers.front(), 0}));
    EXPECT_TRUE(has_audio_wire(result, {src2, 0}, {*mixers.front(), 1}));
    EXPECT_TRUE(has_audio_wire(result, {*mixers.front(), 0}, {dst, 0}));
}

TEST(remove_event_identity_processors, identity_without_output)
{
    using namespace testing;

    processor_mock src;
    event_identity_processor ident(std::in_place_type<float>);

    graph g;

    std::array event_outs{event_port(std::in_place_type<float>)};
    ON_CALL(src, event_outputs())
            .WillByDefault(Return(processor::event_ports{event_outs}));
    g.event.insert({src, 0}, {ident, 0});

    ASSERT_EQ(1, g.event.size());
    remove_event_identity_processors(g);
    EXPECT_TRUE(g.event.empty());
}

TEST(remove_event_identity_processors, identity_without_input)
{
    using namespace testing;

    processor_mock dst;
    event_identity_processor ident(std::in_place_type<float>);

    graph g;

    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.event.insert({ident, 0}, {dst, 0});

    ASSERT_EQ(1, g.event.size());
    remove_event_identity_processors(g);
    EXPECT_TRUE(g.event.empty());
}

TEST(remove_event_identity_processors, identity_between_processors)
{
    using namespace testing;

    processor_mock src;
    processor_mock dst;
    event_identity_processor ident(std::in_place_type<float>);

    graph g;

    std::array event_outs{event_port(std::in_place_type<float>)};
    ON_CALL(src, event_outputs())
            .WillByDefault(Return(processor::event_ports{event_outs}));
    g.event.insert({src, 0}, {ident, 0});
    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.event.insert({ident, 0}, {dst, 0});

    ASSERT_EQ(2, g.event.size());
    remove_event_identity_processors(g);
    EXPECT_EQ(1, g.event.size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst, 0}));
}

TEST(remove_event_identity_processors, identity_in_series_between_processors)
{
    using namespace testing;

    processor_mock src;
    processor_mock dst;
    event_identity_processor ident1(std::in_place_type<float>);
    event_identity_processor ident2(std::in_place_type<float>);

    graph g;

    std::array event_outs{event_port(std::in_place_type<float>)};
    ON_CALL(src, event_outputs())
            .WillByDefault(Return(processor::event_ports{event_outs}));
    g.event.insert({src, 0}, {ident1, 0});
    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.event.insert({ident2, 0}, {dst, 0});

    g.event.insert({ident1, 0}, {ident2, 0});

    ASSERT_EQ(3, g.event.size());
    remove_event_identity_processors(g);
    EXPECT_EQ(1, g.event.size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst, 0}));
}

TEST(remove_event_identity_processors, two_outs_from_identity)
{
    using namespace testing;

    processor_mock src;
    processor_mock dst1;
    processor_mock dst2;
    event_identity_processor ident(std::in_place_type<float>);

    graph g;

    std::array event_outs{event_port(std::in_place_type<float>)};
    ON_CALL(src, event_outputs())
            .WillByDefault(Return(processor::event_ports{event_outs}));
    g.event.insert({src, 0}, {ident, 0});

    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst1, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.event.insert({ident, 0}, {dst1, 0});

    ON_CALL(dst2, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.event.insert({ident, 0}, {dst2, 0});

    ASSERT_EQ(3, g.event.size());
    remove_event_identity_processors(g);
    EXPECT_EQ(2, g.event.size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst1, 0}));
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst2, 0}));
}

TEST(remove_event_identity_processors,
     two_outs_from_identity_one_goes_to_identity)
{
    using namespace testing;

    processor_mock src;
    processor_mock dst1;
    processor_mock dst2;
    event_identity_processor ident1(std::in_place_type<float>);
    event_identity_processor ident2(std::in_place_type<float>);

    graph g;

    std::array event_outs{event_port(std::in_place_type<float>)};
    ON_CALL(src, event_outputs())
            .WillByDefault(Return(processor::event_ports{event_outs}));
    g.event.insert({src, 0}, {ident1, 0});

    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst1, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.event.insert({ident1, 0}, {ident2, 0});
    g.event.insert({ident2, 0}, {dst1, 0});

    ON_CALL(dst2, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.event.insert({ident1, 0}, {dst2, 0});

    ASSERT_EQ(4, g.event.size());
    remove_event_identity_processors(g);
    EXPECT_EQ(2, g.event.size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst1, 0}));
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst2, 0}));
}

TEST(remove_identity_processors, identity_without_output)
{
    using namespace testing;

    processor_mock src;
    auto ident = make_identity_processor();

    graph g;

    ON_CALL(src, num_outputs()).WillByDefault(Return(1));
    g.audio.insert({src, 0}, {*ident, 0});

    ASSERT_EQ(1, g.audio.size());
    remove_identity_processors(g);
    EXPECT_TRUE(g.audio.empty());
}

TEST(remove_identity_processors, identity_without_input)
{
    using namespace testing;

    processor_mock dst;
    auto ident = make_identity_processor();

    graph g;

    ON_CALL(dst, num_inputs()).WillByDefault(Return(1));
    g.audio.insert({*ident, 0}, {dst, 0});

    ASSERT_EQ(1, g.audio.size());
    remove_identity_processors(g);
    EXPECT_TRUE(g.audio.empty());
}

TEST(connect_stereo_components, with_destination_processor)
{
    processor_mock src_proc;
    component_mock src;
    processor_mock dst;

    graph g;

    using testing::Return;

    ON_CALL(src_proc, num_outputs()).WillByDefault(Return(2));
    std::array src_outputs{
            graph_endpoint{.proc = src_proc, .port = 0},
            graph_endpoint{.proc = src_proc, .port = 1}};
    ON_CALL(src, outputs())
            .WillByDefault(Return(component::endpoints(src_outputs)));
    ON_CALL(dst, num_inputs()).WillByDefault(Return(2));

    connect(g, src, dst);
    EXPECT_EQ(2u, g.audio.size());
    EXPECT_TRUE(has_audio_wire(g, {src_proc, 0}, {dst, 0}));
    EXPECT_TRUE(has_audio_wire(g, {src_proc, 1}, {dst, 1}));
    EXPECT_TRUE(g.event.empty());
}

TEST(connect_stereo_components, with_destination_component)
{
    processor_mock src_proc;
    component_mock src;
    processor_mock dst_proc;
    component_mock dst;

    graph g;

    using testing::Return;

    ON_CALL(src_proc, num_outputs()).WillByDefault(Return(2));
    std::array src_outputs{
            graph_endpoint{.proc = src_proc, .port = 0},
            graph_endpoint{.proc = src_proc, .port = 1}};
    ON_CALL(src, outputs())
            .WillByDefault(Return(component::endpoints(src_outputs)));

    ON_CALL(dst_proc, num_inputs()).WillByDefault(Return(2));
    std::array dst_inputs{
            graph_endpoint{.proc = dst_proc, .port = 0},
            graph_endpoint{.proc = dst_proc, .port = 1}};
    ON_CALL(dst, inputs())
            .WillByDefault(Return(component::endpoints(dst_inputs)));

    connect(g, src, dst);
    EXPECT_EQ(2u, g.audio.size());
    EXPECT_TRUE(has_audio_wire(g, {src_proc, 0}, {dst_proc, 0}));
    EXPECT_TRUE(has_audio_wire(g, {src_proc, 1}, {dst_proc, 1}));
    EXPECT_TRUE(g.event.empty());
}

TEST(finalize_graph, connect_two_components_to_one)
{
    fake_processor src_proc1{"src1", 0, 2};
    fake_processor src_proc2{"src2", 0, 2};
    component_mock src1;
    component_mock src2;
    processor_mock dst_proc;
    component_mock dst;

    graph g;

    using testing::Return;

    std::array src1_outputs{
            graph_endpoint{.proc = src_proc1, .port = 0},
            graph_endpoint{.proc = src_proc1, .port = 1}};
    ON_CALL(src1, outputs())
            .WillByDefault(Return(component::endpoints(src1_outputs)));

    std::array src2_outputs{
            graph_endpoint{.proc = src_proc2, .port = 0},
            graph_endpoint{.proc = src_proc2, .port = 1}};
    ON_CALL(src2, outputs())
            .WillByDefault(Return(component::endpoints(src2_outputs)));

    ON_CALL(dst_proc, num_inputs()).WillByDefault(Return(2));
    std::array dst_inputs{
            graph_endpoint{.proc = dst_proc, .port = 0},
            graph_endpoint{.proc = dst_proc, .port = 1}};
    ON_CALL(dst, inputs())
            .WillByDefault(Return(component::endpoints(dst_inputs)));

    connect(g, src1, dst);
    connect(g, src2, dst);

    auto [result, mixers] = finalize_graph(g);

    EXPECT_EQ(2u, mixers.size());
    EXPECT_EQ(6u, result.audio.size());
    EXPECT_TRUE(result.event.empty());

    if (has_audio_wire(result, {src_proc1, 0}, {*mixers[0], 0}))
    {
        EXPECT_TRUE(has_audio_wire(result, {src_proc1, 1}, {*mixers[1], 0}));
        EXPECT_TRUE(has_audio_wire(result, {src_proc2, 0}, {*mixers[0], 1}));
        EXPECT_TRUE(has_audio_wire(result, {src_proc2, 1}, {*mixers[1], 1}));
        EXPECT_TRUE(has_audio_wire(result, {*mixers[0], 0}, {dst_proc, 0}));
        EXPECT_TRUE(has_audio_wire(result, {*mixers[1], 0}, {dst_proc, 1}));
    }
    else
    {
        EXPECT_TRUE(has_audio_wire(result, {src_proc1, 0}, {*mixers[1], 0}));
        EXPECT_TRUE(has_audio_wire(result, {src_proc1, 1}, {*mixers[0], 0}));
        EXPECT_TRUE(has_audio_wire(result, {src_proc2, 0}, {*mixers[1], 1}));
        EXPECT_TRUE(has_audio_wire(result, {src_proc2, 1}, {*mixers[0], 1}));
        EXPECT_TRUE(has_audio_wire(result, {*mixers[1], 0}, {dst_proc, 0}));
        EXPECT_TRUE(has_audio_wire(result, {*mixers[0], 0}, {dst_proc, 1}));
    }
}

} // namespace piejam::audio::engine::test
