// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/process_thread.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

TEST(process_thread, start_is_running_true)
{
    process_thread sut;
    sut.start({}, []() { return std::error_condition(); });
    EXPECT_TRUE(sut.is_running());
}

TEST(process_thread, start_stop_is_running_false)
{
    process_thread sut;
    sut.start({}, []() { return std::error_condition(); });
    sut.stop();
    EXPECT_FALSE(sut.is_running());
}

TEST(process_thread, stop_on_error)
{
    process_thread sut;

    ASSERT_EQ(std::error_condition(), sut.error());
    sut.start({}, []() {
        return std::make_error_condition(std::errc::broken_pipe);
    });

    // wait until thread stops
    for (std::size_t n = 100000; n && sut.is_running(); --n)
        ;

    EXPECT_FALSE(sut.is_running());
    EXPECT_EQ(std::make_error_condition(std::errc::broken_pipe), sut.error());
}

} // namespace piejam::audio::test
