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

#include <piejam/audio/engine/event_buffer.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event_buffer, empty_on_construction)
{
    std::pmr::memory_resource* event_memory{};
    event_buffer<float> sut(event_memory);
    EXPECT_TRUE(sut.empty());
    EXPECT_EQ(0, sut.size());
    EXPECT_EQ(sut.begin(), sut.end());
}

TEST(event_buffer, insert_single_event)
{
    std::pmr::memory_resource* event_memory = std::pmr::get_default_resource();
    event_buffer<float> sut(event_memory);
    sut.insert(5, 23.f);
    EXPECT_FALSE(sut.empty());
    EXPECT_EQ(1u, sut.size());
    ASSERT_NE(sut.begin(), sut.end());

    event<float> const& ev = *sut.begin();
    EXPECT_EQ(5u, ev.offset());
    EXPECT_FLOAT_EQ(23.f, ev.value());
}

TEST(event_buffer, insertion_order_is_kept_if_inserted_at_same_offset)
{
    std::pmr::memory_resource* event_memory = std::pmr::get_default_resource();
    event_buffer<float> sut(event_memory);
    sut.insert(5, 23.f);
    sut.insert(5, 58.f);
    EXPECT_FALSE(sut.empty());
    EXPECT_EQ(2u, sut.size());

    ASSERT_NE(sut.begin(), sut.end());
    event<float> const& ev = *sut.begin();
    EXPECT_EQ(5u, ev.offset());
    EXPECT_FLOAT_EQ(23.f, ev.value());

    ASSERT_NE(std::next(sut.begin()), sut.end());
    event<float> const& ev2 = *std::next(sut.begin());
    EXPECT_EQ(5u, ev2.offset());
    EXPECT_FLOAT_EQ(58.f, ev2.value());
}

TEST(event_buffer, events_are_sorted_on_offset)
{
    std::pmr::memory_resource* event_memory = std::pmr::get_default_resource();
    event_buffer<float> sut(event_memory);
    sut.insert(7, 23.f);
    sut.insert(5, 58.f);
    EXPECT_FALSE(sut.empty());
    EXPECT_EQ(2u, sut.size());

    ASSERT_NE(sut.begin(), sut.end());
    event<float> const& ev = *sut.begin();
    EXPECT_EQ(5u, ev.offset());
    EXPECT_FLOAT_EQ(58.f, ev.value());

    ASSERT_NE(std::next(sut.begin()), sut.end());
    event<float> const& ev2 = *std::next(sut.begin());
    EXPECT_EQ(7u, ev2.offset());
    EXPECT_FLOAT_EQ(23.f, ev2.value());
}

TEST(event_buffer, clear)
{
    std::pmr::memory_resource* event_memory = std::pmr::get_default_resource();
    event_buffer<float> sut(event_memory);
    sut.insert(7, 23.f);
    sut.insert(5, 58.f);
    ASSERT_FALSE(sut.empty());
    ASSERT_EQ(2u, sut.size());

    sut.clear();

    EXPECT_TRUE(sut.empty());
    EXPECT_EQ(0, sut.size());
}

} // namespace piejam::audio::engine::test
