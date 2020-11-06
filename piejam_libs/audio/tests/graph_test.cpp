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

#include <piejam/audio/engine/graph.h>

#include "processor_mock.h"

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(graph_endpoint_less_compare, same_endpoint)
{
    processor_mock proc;
    graph::endpoint sut{proc, 0};

    EXPECT_FALSE(sut < sut);
}

TEST(graph_endpoint_less_compare, same_proc_and_different_ports_first_lower)
{
    processor_mock proc;
    graph::endpoint sut_l{proc, 0};
    graph::endpoint sut_r{proc, 1};

    EXPECT_TRUE(sut_l < sut_r);
    EXPECT_FALSE(sut_r < sut_l);
}

TEST(graph_endpoint_less_compare, same_proc_and_different_ports_second_lower)
{
    processor_mock proc;
    graph::endpoint sut_l{proc, 1};
    graph::endpoint sut_r{proc, 0};

    EXPECT_FALSE(sut_l < sut_r);
    EXPECT_TRUE(sut_r < sut_l);
}

TEST(graph_endpoint_less_compare, different_proc_and_same_ports)
{
    processor_mock proc_l;
    processor_mock proc_r;
    graph::endpoint sut_l{proc_l, 0};
    graph::endpoint sut_r{proc_r, 0};

    EXPECT_TRUE(sut_l < sut_r);
    EXPECT_FALSE(sut_r < sut_l);
}

TEST(graph_endpoint_less_compare,
     different_proc_and_different_ports_first_lower)
{
    processor_mock proc_l;
    processor_mock proc_r;
    graph::endpoint sut_l{proc_l, 0};
    graph::endpoint sut_r{proc_r, 1};

    EXPECT_TRUE(sut_l < sut_r);
    EXPECT_FALSE(sut_r < sut_l);
}

TEST(graph_endpoint_less_compare,
     different_proc_and_different_ports_second_lower)
{
    processor_mock proc_l;
    processor_mock proc_r;
    graph::endpoint sut_l{proc_l, 1};
    graph::endpoint sut_r{proc_r, 0};

    EXPECT_TRUE(sut_l < sut_r);
    EXPECT_FALSE(sut_r < sut_l);
}

TEST(graph_endpoint_equal_compare, same_endpoint)
{
    processor_mock proc;
    graph::endpoint sut{proc, 0};

    EXPECT_EQ(sut, sut);
}

TEST(graph_endpoint_equal_compare, same_proc_different_ports)
{
    processor_mock proc;
    graph::endpoint sut_l{proc, 0};
    graph::endpoint sut_r{proc, 1};

    EXPECT_NE(sut_l, sut_r);
    EXPECT_NE(sut_r, sut_l);
}

TEST(graph_endpoint_equal_compare, different_proc_same_ports)
{
    processor_mock proc_l;
    processor_mock proc_r;
    graph::endpoint sut_l{proc_l, 0};
    graph::endpoint sut_r{proc_r, 0};

    EXPECT_NE(sut_l, sut_r);
    EXPECT_NE(sut_r, sut_l);
}

TEST(graph_endpoint_equal_compare, different_proc_different_ports)
{
    processor_mock proc_l;
    processor_mock proc_r;
    graph::endpoint sut_l{proc_l, 0};
    graph::endpoint sut_r{proc_r, 1};

    EXPECT_NE(sut_l, sut_r);
    EXPECT_NE(sut_r, sut_l);
}

} // namespace piejam::audio::engine::test
