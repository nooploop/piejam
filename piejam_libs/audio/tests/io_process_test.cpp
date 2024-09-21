// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/io_process.h>

#include <piejam/thread/configuration.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(dummy_io_process, is_open_always_false)
{
    auto sut = make_dummy_io_process();
    EXPECT_FALSE(sut->is_open());

    sut->close();
    EXPECT_FALSE(sut->is_open());
}

TEST(dummy_io_process, cant_run)
{
    auto sut = make_dummy_io_process();

    EXPECT_FALSE(sut->is_running());
    sut->start({}, {}, {});

    EXPECT_FALSE(sut->is_running());

    sut->stop();
    EXPECT_FALSE(sut->is_running());

    sut->close();
    EXPECT_FALSE(sut->is_running());
}

TEST(dummy_io_process, stats_always_zero)
{
    auto sut = make_dummy_io_process();

    EXPECT_EQ(0u, sut->xruns());
    EXPECT_FLOAT_EQ(0.f, sut->cpu_load());

    sut->start({}, {}, {});

    EXPECT_EQ(0u, sut->xruns());
    EXPECT_FLOAT_EQ(0.f, sut->cpu_load());

    sut->stop();

    EXPECT_EQ(0u, sut->xruns());
    EXPECT_FLOAT_EQ(0.f, sut->cpu_load());

    sut->close();

    EXPECT_EQ(0u, sut->xruns());
    EXPECT_FLOAT_EQ(0.f, sut->cpu_load());
}

} // namespace piejam::audio::test
