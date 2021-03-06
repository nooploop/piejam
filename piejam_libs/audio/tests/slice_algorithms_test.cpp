// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/slice_algorithms.h>

#include <piejam/audio/simd.h>

#include <gmock/gmock-matchers.h>
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
    slice<float> x{1.f};
    alignas(mipp::RequiredAlignment)
            std::array buf{2.f, 3.f, 5.f, 7.f, 11.f, 13.f, 17.f, 19.f};
    slice<float> y(buf);
    alignas(mipp::RequiredAlignment) decltype(buf) out{};
    std::array expected{3.f, 4.f, 6.f, 8.f, 12.f, 14.f, 18.f, 20.f};

    auto res = add(x, y, {out});

    ASSERT_TRUE(res.is_buffer());
    auto res_buffer = res.buffer();
    EXPECT_EQ(out.data(), res_buffer.data());
    EXPECT_EQ(out.size(), res_buffer.size());
    EXPECT_TRUE(std::ranges::equal(res_buffer, expected));
}

TEST(slice_algorithms_add, adding_buffers_will_write_to_out)
{
    alignas(mipp::RequiredAlignment)
            std::array buf_x{2.f, 3.f, 5.f, 7.f, 2.f, 3.f, 5.f, 7.f};
    slice<float> x{buf_x};
    alignas(mipp::RequiredAlignment)
            std::array buf_y{3.f, 4.f, 5.f, 9.f, 3.f, 4.f, 5.f, 9.f};
    slice<float> y{buf_y};
    alignas(mipp::RequiredAlignment) decltype(buf_x) out{};
    std::array expected{5.f, 7.f, 10.f, 16.f, 5.f, 7.f, 10.f, 16.f};

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
    alignas(mipp::RequiredAlignment) std::array buf{2, 3, 5, 7};
    slice<int> x(buf);
    slice<int> y;

    auto res = multiply(x, y, {});

    ASSERT_TRUE(res.is_constant());
    EXPECT_EQ(0, res.constant());
}

TEST(slice_algorithms_multiply, multiply_buffer_by_one_will_result_in_buffer)
{
    alignas(mipp::RequiredAlignment) std::array buf{2, 3, 5, 7};
    slice<int> x(buf);
    slice<int> y(1);

    auto res = multiply(x, y, {});

    ASSERT_TRUE(res.is_buffer());
    auto res_buffer = res.buffer();
    EXPECT_EQ(buf.data(), res_buffer.data());
    EXPECT_EQ(buf.size(), res_buffer.size());
}

TEST(slice_interleave_2_slices, both_constant)
{
    slice<float> l(1.f);
    slice<float> r(-1.f);
    std::array<float, 6> out{};
    interleave(l, r, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(1.f, -1.f, 1.f, -1.f, 1.f, -1.f))(out));
}

TEST(slice_interleave_2_slices, buffer_and_constant)
{
    std::array<float, 3> buf{1.f, 1.f, 1.f};
    slice<float> l(buf);
    slice<float> r(-1.f);
    std::array<float, 6> out{};
    interleave(l, r, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(1.f, -1.f, 1.f, -1.f, 1.f, -1.f))(out));
}

TEST(slice_interleave_2_slices, constant_and_buffer)
{
    std::array<float, 3> buf{-1.f, -1.f, -1.f};
    slice<float> l(1.f);
    slice<float> r(buf);
    std::array<float, 6> out{};
    interleave(l, r, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(1.f, -1.f, 1.f, -1.f, 1.f, -1.f))(out));
}

TEST(slice_interleave_2_slices, two_buffer)
{
    alignas(mipp::RequiredAlignment) std::array<float, 8> l_buf;
    l_buf.fill(1.f);
    alignas(mipp::RequiredAlignment) std::array<float, 8> r_buf;
    r_buf.fill(-1.f);
    slice<float> l(l_buf);
    slice<float> r(r_buf);
    alignas(mipp::RequiredAlignment) std::array<float, 16> out{};
    interleave(l, r, {out});

    bool left{true};
    for (auto const v : out)
    {
        EXPECT_EQ(left ? 1.f : -1.f, v);
        left = !left;
    }
}

TEST(slice_clamp, constant)
{
    slice<float> l(1.5f);
    auto l_res = engine::clamp(l, -1.f, 1.f, {});
    ASSERT_TRUE(l_res.is_constant());
    EXPECT_EQ(1.f, l_res.constant());

    slice<float> r(-1.5f);
    auto r_res = engine::clamp(r, -1.f, 1.f, {});
    ASSERT_TRUE(r_res.is_constant());
    EXPECT_EQ(-1.f, r_res.constant());

    slice<float> m(0.5f);
    auto m_res = engine::clamp(m, -1.f, 1.f, {});
    ASSERT_TRUE(m_res.is_constant());
    EXPECT_EQ(0.5f, m_res.constant());
}

TEST(slice_clamp, buffer)
{
    alignas(mipp::RequiredAlignment)
            std::array buf{-1.5f, 1.5f, 0.5f, 23.f, -17.f, 0.23f, 0.f, -0.25f};
    slice<float> s(buf);
    auto res = engine::clamp(s, -1.f, 1.f, {buf});
    ASSERT_TRUE(res.is_buffer());
    EXPECT_EQ(buf.size(), res.buffer().size());
    EXPECT_EQ(buf.data(), res.buffer().data());

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(
            ElementsAre(-1.f, 1.f, 0.5f, 1.f, -1.f, 0.23f, 0.f, -0.25f))(buf));
}

} // namespace piejam::audio::engine::test
