// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_buffer_memory.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event_buffer_memory, can_allocate)
{
    event_buffer_memory sut(128);
    int* mem = static_cast<int*>(sut.memory_resource().allocate(sizeof(int)));
    EXPECT_NE(nullptr, mem);
    *mem = 23;
}

TEST(event_buffer_memory, can_allocate_over_max)
{
    event_buffer_memory sut(128);
    void* mem = sut.memory_resource().allocate(256);
    EXPECT_NE(nullptr, mem);
}

} // namespace piejam::audio::engine::test
