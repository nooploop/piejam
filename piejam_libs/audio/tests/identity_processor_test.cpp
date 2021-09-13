// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/identity_processor.h>

#include <piejam/audio/engine/processor.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(identity_processor, properties)
{
    auto sut = make_identity_processor();

    EXPECT_EQ(1u, sut->num_inputs());
    EXPECT_EQ(1u, sut->num_outputs());
    EXPECT_TRUE(sut->event_inputs().empty());
    EXPECT_TRUE(sut->event_outputs().empty());
}

TEST(identity_processor, is_identity_processor)
{
    auto sut = make_identity_processor();

    EXPECT_TRUE(is_identity_processor(*sut));
}

} // namespace piejam::audio::engine::test
