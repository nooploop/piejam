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

    sut.add<float>();
    EXPECT_FALSE(sut.empty());
    ASSERT_EQ(1u, sut.size());
    auto& buf = sut.get<float>(0);
    EXPECT_NE(nullptr, std::addressof(buf));
}

TEST(event_output_buffers, clear_buffers)
{
    event_output_buffers sut;
    sut.add<float>();
    ASSERT_EQ(1u, sut.size());
    auto& buf = sut.get<float>(0);
    buf.insert(5, 23.f);
    ASSERT_FALSE(buf.empty());

    sut.clear_buffers();

    EXPECT_TRUE(buf.empty());
}

} // namespace piejam::audio::engine::test
