// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/graph_algorithms.h>

#include "processor_mock.h"

#include <piejam/audio/engine/event_identity_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/slice.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(connect, connect_to_already_connected_will_insert_a_mixer)
{
    using namespace testing;

    processor_mock src1;
    processor_mock src2;
    processor_mock dst;
    std::vector<std::unique_ptr<processor>> mixers;
    graph sut;

    ON_CALL(src1, num_outputs()).WillByDefault(Return(1));
    ON_CALL(src2, num_outputs()).WillByDefault(Return(1));
    ON_CALL(dst, num_inputs()).WillByDefault(Return(1));

    sut.add_wire({src1, 0}, {dst, 0});
    EXPECT_TRUE(has_wire(sut, {src1, 0}, {dst, 0}));

    connect(sut, {src2, 0}, {dst, 0}, mixers);
    EXPECT_FALSE(has_wire(sut, {src1, 0}, {dst, 0}));
    ASSERT_EQ(1u, mixers.size());
    EXPECT_TRUE(is_mix_processor(*mixers.front()));
    EXPECT_EQ(2u, mixers.front()->num_inputs());
    EXPECT_TRUE(has_wire(sut, {src1, 0}, {*mixers.front(), 0}));
    EXPECT_TRUE(has_wire(sut, {src2, 0}, {*mixers.front(), 1}));
    EXPECT_TRUE(has_wire(sut, {*mixers.front(), 0}, {dst, 0}));
}

TEST(connect,
     connect_to_already_connected_and_connected_is_a_mixer_will_replace_the_mixer_with_new_one_with_additional_input)
{
    using namespace testing;

    processor_mock src1;
    processor_mock src2;
    processor_mock src3;
    processor_mock dst;
    std::vector<std::unique_ptr<processor>> mixers;
    graph sut;

    ON_CALL(src1, num_outputs()).WillByDefault(Return(1));
    ON_CALL(src2, num_outputs()).WillByDefault(Return(1));
    ON_CALL(src3, num_outputs()).WillByDefault(Return(1));
    ON_CALL(dst, num_inputs()).WillByDefault(Return(1));

    sut.add_wire({src1, 0}, {dst, 0});
    EXPECT_TRUE(has_wire(sut, {src1, 0}, {dst, 0}));
    connect(sut, {src2, 0}, {dst, 0}, mixers);
    connect(sut, {src3, 0}, {dst, 0}, mixers);

    EXPECT_FALSE(has_wire(sut, {src1, 0}, {dst, 0}));
    ASSERT_EQ(1u, mixers.size());
    EXPECT_TRUE(is_mix_processor(*mixers.front()));
    EXPECT_EQ(3u, mixers.front()->num_inputs());
    EXPECT_TRUE(has_wire(sut, {src1, 0}, {*mixers.front(), 0}));
    EXPECT_TRUE(has_wire(sut, {src2, 0}, {*mixers.front(), 1}));
    EXPECT_TRUE(has_wire(sut, {src3, 0}, {*mixers.front(), 2}));
    EXPECT_TRUE(has_wire(sut, {*mixers.front(), 0}, {dst, 0}));
}

TEST(bypass_event_identity_processors, identity_without_output)
{
    using namespace testing;

    processor_mock src;
    event_identity_processor ident(std::in_place_type<float>);

    graph g;

    std::array event_outs{event_port(std::in_place_type<float>)};
    ON_CALL(src, event_outputs())
            .WillByDefault(Return(processor::event_ports{event_outs}));
    g.add_event_wire({src, 0}, {ident, 0});

    ASSERT_EQ(1, g.event_wires().size());
    bypass_event_identity_processors(g);
    EXPECT_TRUE(g.event_wires().empty());
}

TEST(bypass_event_identity_processors, identity_without_input)
{
    using namespace testing;

    processor_mock dst;
    event_identity_processor ident(std::in_place_type<float>);

    graph g;

    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.add_event_wire({ident, 0}, {dst, 0});

    ASSERT_EQ(1, g.event_wires().size());
    bypass_event_identity_processors(g);
    EXPECT_TRUE(g.event_wires().empty());
}

TEST(bypass_event_identity_processors, identity_between_processors)
{
    using namespace testing;

    processor_mock src;
    processor_mock dst;
    event_identity_processor ident(std::in_place_type<float>);

    graph g;

    std::array event_outs{event_port(std::in_place_type<float>)};
    ON_CALL(src, event_outputs())
            .WillByDefault(Return(processor::event_ports{event_outs}));
    g.add_event_wire({src, 0}, {ident, 0});
    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.add_event_wire({ident, 0}, {dst, 0});

    ASSERT_EQ(2, g.event_wires().size());
    bypass_event_identity_processors(g);
    EXPECT_EQ(1, g.event_wires().size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst, 0}));
}

TEST(bypass_event_identity_processors, identity_in_series_between_processors)
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
    g.add_event_wire({src, 0}, {ident1, 0});
    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.add_event_wire({ident2, 0}, {dst, 0});

    g.add_event_wire({ident1, 0}, {ident2, 0});

    ASSERT_EQ(3, g.event_wires().size());
    bypass_event_identity_processors(g);
    EXPECT_EQ(1, g.event_wires().size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst, 0}));
}

TEST(bypass_event_identity_processors, two_outs_from_identity)
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
    g.add_event_wire({src, 0}, {ident, 0});

    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst1, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.add_event_wire({ident, 0}, {dst1, 0});

    ON_CALL(dst2, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.add_event_wire({ident, 0}, {dst2, 0});

    ASSERT_EQ(3, g.event_wires().size());
    bypass_event_identity_processors(g);
    EXPECT_EQ(2, g.event_wires().size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst1, 0}));
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst2, 0}));
}

TEST(bypass_event_identity_processors,
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
    g.add_event_wire({src, 0}, {ident1, 0});

    std::array event_ins{event_port(std::in_place_type<float>)};
    ON_CALL(dst1, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.add_event_wire({ident1, 0}, {ident2, 0});
    g.add_event_wire({ident2, 0}, {dst1, 0});

    ON_CALL(dst2, event_inputs())
            .WillByDefault(Return(processor::event_ports{event_ins}));
    g.add_event_wire({ident1, 0}, {dst2, 0});

    ASSERT_EQ(4, g.event_wires().size());
    bypass_event_identity_processors(g);
    EXPECT_EQ(2, g.event_wires().size());
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst1, 0}));
    EXPECT_TRUE(has_event_wire(g, {src, 0}, {dst2, 0}));
}

} // namespace piejam::audio::engine::test
