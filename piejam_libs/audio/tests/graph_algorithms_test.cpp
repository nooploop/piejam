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

#include <piejam/audio/engine/graph_algorithms.h>

#include "processor_mock.h"

#include <piejam/audio/engine/mix_processor.h>

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

} // namespace piejam::audio::engine::test
