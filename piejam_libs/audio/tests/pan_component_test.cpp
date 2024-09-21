// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/pan_balance.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/pan_balance_processor.h>
#include <piejam/audio/engine/processor.h>

#include <gtest/gtest.h>

namespace piejam::audio::components::test
{

TEST(pan_component, pan_io)
{
    auto sut = make_pan(engine::make_pan_processor());
    EXPECT_EQ(1u, sut->inputs().size());
    EXPECT_EQ(2u, sut->outputs().size());
    EXPECT_EQ(1u, sut->event_inputs().size());
    EXPECT_EQ(0u, sut->event_outputs().size());
}

TEST(pan_component, volume_pan_io)
{
    auto sut = make_pan(engine::make_volume_pan_processor());
    EXPECT_EQ(1u, sut->inputs().size());
    EXPECT_EQ(2u, sut->outputs().size());
    EXPECT_EQ(2u, sut->event_inputs().size());
    EXPECT_EQ(0u, sut->event_outputs().size());
}

TEST(pan_component, balance_io)
{
    auto sut = make_balance(engine::make_balance_processor());
    EXPECT_EQ(2u, sut->inputs().size());
    EXPECT_EQ(2u, sut->outputs().size());
    EXPECT_EQ(1u, sut->event_inputs().size());
    EXPECT_EQ(0u, sut->event_outputs().size());
}

TEST(pan_component, volume_balance_io)
{
    auto sut = make_balance(engine::make_volume_balance_processor());
    EXPECT_EQ(2u, sut->inputs().size());
    EXPECT_EQ(2u, sut->outputs().size());
    EXPECT_EQ(2u, sut->event_inputs().size());
    EXPECT_EQ(0u, sut->event_outputs().size());
}

} // namespace piejam::audio::components::test
