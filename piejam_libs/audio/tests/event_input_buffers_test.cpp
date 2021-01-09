// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_input_buffers.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event_input_buffers, empty_after_construction)
{
    event_input_buffers sut;
    EXPECT_EQ(0, sut.size());
}

TEST(event_input_buffers, add_will_add_an_empty_event_buffer_reference)
{
    event_input_buffers sut;
    event_port port(std::in_place_type<float>, {});
    sut.add(port);
    sut.add(port);
    sut.add(port);
    ASSERT_EQ(3, sut.size());
    EXPECT_TRUE(sut.get<float>(0).empty());
    EXPECT_TRUE(sut.get<float>(1).empty());
    EXPECT_TRUE(sut.get<float>(2).empty());
}

TEST(event_input_buffers, buffer_can_be_set_and_retrieved)
{
    std::pmr::memory_resource* mem{};
    event_buffer<float> buf(mem);
    event_input_buffers sut;
    event_port port(std::in_place_type<float>, {});
    sut.add(port);
    ASSERT_EQ(1, sut.size());
    sut.set(0, buf);
    EXPECT_EQ(std::addressof(buf), &sut.get<float>(0));
}

} // namespace piejam::audio::engine::test
