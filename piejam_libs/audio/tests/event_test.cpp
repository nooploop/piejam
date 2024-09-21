// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event, offset)
{
    event<float> ev(5, 23.f);
    EXPECT_EQ(5u, ev.offset());
}

TEST(event, value)
{
    event<float> ev(5, 23.f);
    EXPECT_EQ(23.f, ev.value());
}

} // namespace piejam::audio::engine::test
