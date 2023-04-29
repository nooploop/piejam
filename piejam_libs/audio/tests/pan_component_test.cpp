// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/pan_balance.h>

#include <piejam/audio/engine/component.h>

#include <gtest/gtest.h>

namespace piejam::audio::components::test
{

TEST(pan_component, stereo_in_out)
{
    auto sut = make_pan();
    EXPECT_EQ(2u, sut->inputs().size());
    EXPECT_EQ(2u, sut->outputs().size());
}

TEST(pan_component, event_io)
{
    auto sut = make_pan();
    EXPECT_EQ(1u, sut->event_inputs().size());
    EXPECT_EQ(0u, sut->event_outputs().size());
}

} // namespace piejam::audio::components::test
