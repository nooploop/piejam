// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/slice_algorithms.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(slice_algorithms_add, adding_constants_results_in_a_constant)
{
    slice<float> x(23.f);
    slice<float> y(58.f);

    auto res = add(x, y, {});

    ASSERT_TRUE(res.is_constant());
    EXPECT_FLOAT_EQ(23.f + 58.f, res.constant());
}

TEST(slice_algorithms_add, adding_zero_to_buffer_will_result_in_the_buffer)
{
    slice<float> x;
    std::array buf{2.f, 3.f, 5.f};
    slice<float> y(buf);

    auto res = add(x, y, {});

    ASSERT_TRUE(res.is_buffer());
    auto res_buffer = res.buffer();
    EXPECT_EQ(buf.data(), res_buffer.data());
    EXPECT_EQ(buf.size(), res_buffer.size());
}

TEST(slice_algorithms_add, adding_non_zero_to_buffer_will_write_to_out)
{
    slice<int> x{1};
    std::array buf{2, 3, 5, 7};
    slice<int> y(buf);
    decltype(buf) out{};
    std::array expected{3, 4, 6, 8};

    auto res = add(x, y, {out});

    ASSERT_TRUE(res.is_buffer());
    auto res_buffer = res.buffer();
    EXPECT_EQ(out.data(), res_buffer.data());
    EXPECT_EQ(out.size(), res_buffer.size());
    EXPECT_TRUE(std::ranges::equal(res_buffer, expected));
}

TEST(slice_algorithms_add, adding_buffers_will_write_to_out)
{
    std::array buf_x{2, 3, 5, 7};
    slice<int> x{buf_x};
    std::array buf_y{3, 4, 5, 9};
    slice<int> y{buf_y};
    decltype(buf_x) out{};
    std::array expected{5, 7, 10, 16};

    auto res = add(x, y, {out});

    ASSERT_TRUE(res.is_buffer());
    auto res_buffer = res.buffer();
    EXPECT_EQ(out.data(), res_buffer.data());
    EXPECT_EQ(out.size(), res_buffer.size());
    EXPECT_TRUE(std::ranges::equal(res_buffer, expected));
}

TEST(slice_algorithms_multiply,
     multiply_buffer_by_zero_will_result_in_zero_constant)
{
    std::array buf{2, 3, 5, 7};
    slice<int> x(buf);
    slice<int> y;

    auto res = multiply(x, y, {});

    ASSERT_TRUE(res.is_constant());
    EXPECT_EQ(0, res.constant());
}

TEST(slice_algorithms_multiply, multiply_buffer_by_one_will_result_in_buffer)
{
    std::array buf{2, 3, 5, 7};
    slice<int> x(buf);
    slice<int> y(1);

    auto res = multiply(x, y, {});

    ASSERT_TRUE(res.is_buffer());
    auto res_buffer = res.buffer();
    EXPECT_EQ(buf.data(), res_buffer.data());
    EXPECT_EQ(buf.size(), res_buffer.size());
}

} // namespace piejam::audio::engine::test
