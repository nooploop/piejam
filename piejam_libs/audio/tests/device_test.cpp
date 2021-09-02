// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/device.h>

#include <piejam/thread/configuration.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(dummy_device, is_open_always_false)
{
    dummy_device sut;
    EXPECT_FALSE(sut.is_open());

    sut.close();
    EXPECT_FALSE(sut.is_open());
}

TEST(dummy_device, cant_run)
{
    dummy_device sut;

    EXPECT_FALSE(sut.is_running());
    sut.start({}, {}, {});

    EXPECT_FALSE(sut.is_running());

    sut.stop();
    EXPECT_FALSE(sut.is_running());

    sut.close();
    EXPECT_FALSE(sut.is_running());
}

TEST(dummy_device, stats_always_zero)
{
    dummy_device sut;

    EXPECT_EQ(0u, sut.xruns());
    EXPECT_FLOAT_EQ(0.f, sut.cpu_load());

    sut.start({}, {}, {});

    EXPECT_EQ(0u, sut.xruns());
    EXPECT_FLOAT_EQ(0.f, sut.cpu_load());

    sut.stop();

    EXPECT_EQ(0u, sut.xruns());
    EXPECT_FLOAT_EQ(0.f, sut.cpu_load());

    sut.close();

    EXPECT_EQ(0u, sut.xruns());
    EXPECT_FLOAT_EQ(0.f, sut.cpu_load());
}

} // namespace piejam::audio::test
