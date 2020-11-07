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

#include <piejam/audio/engine/event_input_buffers.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event_input_buffers, empty_after_construction)
{
    event_input_buffers sut;
    EXPECT_EQ(0, sut.size());
}

TEST(event_input_buffers, has_the_size_set_on_construction)
{
    event_input_buffers sut(5);
    EXPECT_EQ(5, sut.size());
}

TEST(event_input_buffers, buffers_are_empty_after_construction)
{
    event_input_buffers sut(3);
    ASSERT_EQ(3, sut.size());
    EXPECT_TRUE(sut.get<float>(0).empty());
    EXPECT_TRUE(sut.get<float>(1).empty());
    EXPECT_TRUE(sut.get<float>(2).empty());
}

TEST(event_input_buffers, buffer_can_be_set_and_retrieved)
{
    std::pmr::memory_resource* mem{};
    event_buffer<float> buf(mem);
    event_input_buffers sut(1);
    ASSERT_EQ(1, sut.size());
    sut.set(0, buf);
    EXPECT_EQ(std::addressof(buf), &sut.get<float>(0));
}

} // namespace piejam::audio::engine::test
