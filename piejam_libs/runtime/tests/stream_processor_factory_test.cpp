// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/stream_processor_factory.h>

#include <piejam/audio/engine/stream_processor.h>

#include <gtest/gtest.h>

namespace piejam::runtime::processors::test
{

TEST(stream_processor_factory, make_and_verify_num_inputs)
{
    stream_processor_factory sut;
    auto proc = sut.make_processor(audio_stream_id::generate(), 5, 1024);

    ASSERT_TRUE(proc);
    EXPECT_EQ(5u, proc->num_inputs());
}

TEST(stream_processor_factory, make_and_find)
{
    stream_processor_factory sut;
    auto stream_id = audio_stream_id::generate();
    auto proc = sut.make_processor(stream_id, 5, 1024);

    auto proc_found = sut.find_processor(stream_id);

    ASSERT_TRUE(proc);
    EXPECT_EQ(proc.get(), proc_found.get());
}

TEST(stream_processor_factory, find_expired)
{
    stream_processor_factory sut;
    auto stream_id = audio_stream_id::generate();
    sut.make_processor(stream_id, 5, 1024);

    auto proc_found = sut.find_processor(stream_id);

    EXPECT_FALSE(proc_found);
}

TEST(stream_processor_factory, find_after_clear_expired)
{
    stream_processor_factory sut;
    auto stream_id = audio_stream_id::generate();
    auto proc = sut.make_processor(stream_id, 5, 1024);

    sut.clear_expired();

    auto proc_found = sut.find_processor(stream_id);

    ASSERT_TRUE(proc);
    EXPECT_EQ(proc.get(), proc_found.get());
}

TEST(stream_processor_factory, find_expired_after_clear_expired)
{
    stream_processor_factory sut;
    auto stream_id = audio_stream_id::generate();

    sut.clear_expired();

    auto proc_found = sut.find_processor(stream_id);

    EXPECT_FALSE(proc_found);
}

} // namespace piejam::runtime::processors::test
