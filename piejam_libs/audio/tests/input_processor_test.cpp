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

#include <piejam/audio/engine/input_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>

#include <piejam/container/table.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(input_processor, input_table_is_propagated_to_outputs)
{
    container::table<float> data(2, 16);
    input_processor sut(2);
    sut.set_input(as_const(data.rows()));

    std::array<float, 16> out_buf{};
    std::vector<std::span<float>> outputs{out_buf, out_buf};
    std::vector<std::span<float const>> results(2);
    sut.process({}, outputs, results, {0}, {});

    EXPECT_EQ(results[0].data(), data.rows()[0].data());
    EXPECT_EQ(results[0].size(), data.rows()[0].size());
    EXPECT_EQ(results[1].data(), data.rows()[1].data());
    EXPECT_EQ(results[1].size(), data.rows()[1].size());
}

} // namespace piejam::audio::engine::test
