// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_output_buffers.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event_output_buffers, empty_after_construction)
{
    event_output_buffers sut;
    EXPECT_TRUE(sut.empty());
    EXPECT_EQ(0, sut.size());
}

TEST(event_output_buffers, add_and_get)
{
    event_output_buffers sut;
    ASSERT_TRUE(sut.empty());

    sut.add(event_port(std::in_place_type<float>, {}));
    EXPECT_FALSE(sut.empty());
    ASSERT_EQ(1u, sut.size());
    auto& buf = sut.get<float>(0);
    EXPECT_NE(nullptr, std::addressof(buf));
}

TEST(event_output_buffers, clear_buffers)
{
    event_output_buffers sut;
    sut.add(event_port(std::in_place_type<float>, {}));
    ASSERT_EQ(1u, sut.size());
    auto& buf = sut.get<float>(0);
    buf.insert(5, 23.f);
    ASSERT_FALSE(buf.empty());

    sut.clear_buffers();

    EXPECT_TRUE(buf.empty());
}

} // namespace piejam::audio::engine::test
