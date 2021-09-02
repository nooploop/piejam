// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_identity_processor.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event_identity_processor, verify_properties)
{
    auto sut = make_event_identity_processor<float>();

    EXPECT_TRUE(is_event_identity_processor(*sut));
    ASSERT_EQ(1u, sut->event_inputs().size());
    ASSERT_EQ(1u, sut->event_outputs().size());
    EXPECT_EQ(sut->event_inputs()[0].type(), sut->event_outputs()[0].type());
    EXPECT_EQ(0u, sut->num_inputs());
    EXPECT_EQ(0u, sut->num_outputs());
}

} // namespace piejam::audio::engine::test
