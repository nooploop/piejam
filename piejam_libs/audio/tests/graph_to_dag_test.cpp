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

#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_to_dag.h>
#include <piejam/audio/engine/input_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/audio/engine/output_processor.h>
#include <piejam/container/table.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(graph_to_dag, mix_two_inputs_to_one_output)
{
    input_processor in_proc(2);
    auto mix_proc = make_mix_processor(2);
    output_processor out_proc(1);

    graph g;

    g.add_wire(in_proc, 0, *mix_proc, 0);
    g.add_wire(in_proc, 1, *mix_proc, 1);
    g.add_wire(*mix_proc, 0, out_proc, 0);

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g, 8, buffer_size);

    container::table<float> in_table(2, 1);
    container::table<float> out_table(1, 1);

    in_proc.set_input(as_const(in_table.rows()));
    out_proc.set_output(out_table.rows());

    in_table.rows()[0][0] = 0.3f;
    in_table.rows()[1][0] = 0.4f;

    d();

    EXPECT_FLOAT_EQ(0.7f, out_table.rows()[0][0]);
}

TEST(graph_to_dag, mix_two_silence_inputs_to_one_output)
{
    auto mix_proc = make_mix_processor(2);
    output_processor out_proc(1);

    graph g;

    g.add_wire(*mix_proc, 0, out_proc, 0);

    std::size_t buffer_size = 1;
    auto d = graph_to_dag(g, 8, buffer_size);

    container::table<float> out_table(1, 1);

    out_proc.set_output(out_table.rows());

    d();

    EXPECT_FLOAT_EQ(0.f, out_table.rows()[0][0]);
}

} // namespace piejam::audio::engine::test
