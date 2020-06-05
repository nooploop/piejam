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
