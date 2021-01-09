// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/level_meter.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(level_meter, default_ctor_level_is_zero)
{
    level_meter sut(4, 4);
    EXPECT_EQ(0.f, sut.get());
    EXPECT_EQ(0.f, sut.get_rms());
}

TEST(level_meter, add_one_value)
{
    level_meter sut(4, 4);
    sut.push_back(1.f);
    EXPECT_FLOAT_EQ(1.f, sut.get());
    EXPECT_FLOAT_EQ(0.5f, sut.get_rms());
}

TEST(level_meter, add_two_values)
{
    level_meter sut(4, 4);
    sut.push_back(1.f);
    sut.push_back(0.5f);
    EXPECT_FLOAT_EQ(0.75f, sut.get());
    EXPECT_FLOAT_EQ(0.55901699f, sut.get_rms());
}

} // namespace piejam::audio::test
