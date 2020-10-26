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

#include <piejam/audio/engine/amplify_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

struct amplify_processor_test : ::testing::Test
{
    std::array<float, 128> in_buf_memory;
    std::span<float const> in_buf{in_buf_memory};
    std::vector<std::reference_wrapper<std::span<float const> const>> in_bufs{
            in_buf};
    std::array<float, 128> out_buf_memory{};
    std::vector<std::span<float>> out_bufs{out_buf_memory};
    std::vector<std::span<float const>> res_bufs{out_bufs[0]};
    event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_mem{&ev_buf_mem.memory_resource()};
    event_buffer<float> ev_in_buf{ev_mem};
    event_input_buffers ev_in_bufs{1};

    amplify_processor_test()
    {
        in_buf_memory.fill(1.f);
        ev_in_bufs.set(0, ev_in_buf);
    }
};

TEST_F(amplify_processor_test,
       result_is_input_when_factor_is_one_and_event_is_not_connected)
{
    auto sut = make_amplify_processor();
    sut->process({in_bufs, out_bufs, res_bufs, {1}, {}, 128});

    EXPECT_EQ(res_bufs[0].data(), in_bufs[0].get().data());
    EXPECT_EQ(res_bufs[0].size(), in_bufs[0].get().size());
}

TEST_F(amplify_processor_test,
       result_is_input_when_factor_is_one_and_event_buf_is_empty)
{
    auto sut = make_amplify_processor();

    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});

    EXPECT_EQ(res_bufs[0].data(), in_bufs[0].get().data());
    EXPECT_EQ(res_bufs[0].size(), in_bufs[0].get().size());
}

TEST_F(amplify_processor_test, buffer_is_smoothly_amplified_after_event)
{
    ev_in_buf.insert(0, 2.f);

    auto sut = make_amplify_processor();
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});

    ASSERT_NE(nullptr, res_bufs[0].data());
    ASSERT_EQ(128u, res_bufs[0].size());
    EXPECT_EQ(
            res_bufs[0].end(),
            std::ranges::adjacent_find(res_bufs[0], [](float l, float r) {
                return l >= r;
            }));
    EXPECT_EQ(1.f, res_bufs[0][0]);
    EXPECT_GT(2.f, res_bufs[0][127]);
}

TEST_F(amplify_processor_test, buffer_is_not_amplified_before_event)
{
    ev_in_buf.insert(2, 2.f);

    auto sut = make_amplify_processor();
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});

    ASSERT_NE(nullptr, res_bufs[0].data());
    ASSERT_EQ(128u, res_bufs[0].size());
    EXPECT_EQ(
            res_bufs[0].end(),
            std::ranges::adjacent_find(
                    std::next(
                            res_bufs[0].begin(),
                            2), // amplification starts from 3rd sample
                    res_bufs[0].end(),
                    [](float l, float r) { return l >= r; }));
    EXPECT_EQ(1.f, res_bufs[0][0]);
    EXPECT_EQ(1.f, res_bufs[0][1]);
    EXPECT_EQ(1.f, res_bufs[0][2]);
    EXPECT_GT(2.f, res_bufs[0][127]);
}

TEST_F(amplify_processor_test, buffer_is_processed_sliced_with_multiple_events)
{
    ev_in_buf.insert(2, 2.f);
    ev_in_buf.insert(4, 1.f);

    auto sut = make_amplify_processor();
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});

    ASSERT_NE(nullptr, res_bufs[0].data());
    ASSERT_EQ(128u, res_bufs[0].size());
    EXPECT_EQ(1.f, res_bufs[0][0]);
    EXPECT_EQ(1.f, res_bufs[0][1]);
    EXPECT_EQ(1.f, res_bufs[0][2]);
    EXPECT_LT(res_bufs[0][2], res_bufs[0][3]);
    EXPECT_LT(res_bufs[0][3], res_bufs[0][4]);
    EXPECT_GT(res_bufs[0][4], res_bufs[0][5]);
    EXPECT_GT(res_bufs[0][5], res_bufs[0][6]);
}

TEST_F(amplify_processor_test, result_is_silence_when_factor_is_zero)
{
    ev_in_buf.insert(0, 0.f);

    auto sut = make_amplify_processor();
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});
    ev_in_buf.clear();

    ASSERT_EQ(out_bufs[0].data(), res_bufs[0].data());
    ASSERT_EQ(128u, res_bufs[0].size());

    // smoothing is finished in the previous process, now we should have silence
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});

    EXPECT_EQ(nullptr, res_bufs[0].data());
    EXPECT_EQ(0, res_bufs[0].size());
}

TEST_F(amplify_processor_test, factor_stays_after_smoothing)
{
    ev_in_buf.insert(0, 2.f);

    auto sut = make_amplify_processor();
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});
    ev_in_buf.clear();

    ASSERT_EQ(out_bufs[0].data(), res_bufs[0].data());
    ASSERT_EQ(128u, res_bufs[0].size());

    // smoothing is finished in the previous process,
    // now factor should be applied to all samples in next process
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});

    EXPECT_EQ(out_bufs[0].data(), res_bufs[0].data());
    EXPECT_EQ(128u, res_bufs[0].size());
    EXPECT_TRUE(
            std::ranges::all_of(res_bufs[0], [](float x) { return x == 2.f; }));
}

TEST_F(amplify_processor_test,
       result_is_silence_for_empty_input_and_possible_events)
{
    in_buf = {};
    ev_in_buf.insert(10, 2.f);

    auto sut = make_amplify_processor();
    sut->process({in_bufs, out_bufs, res_bufs, ev_in_bufs, {}, 128});

    EXPECT_EQ(nullptr, res_bufs[0].data());
    EXPECT_EQ(0, res_bufs[0].size());
}

} // namespace piejam::audio::engine::test
