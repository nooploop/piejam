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

    ASSERT_TRUE(res.is_span());
    auto res_buffer = res.span();
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

    ASSERT_TRUE(res.is_span());
    auto res_buffer = res.span();
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

    ASSERT_TRUE(res.is_span());
    auto res_buffer = res.span();
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

    ASSERT_TRUE(res.is_span());
    auto res_buffer = res.span();
    EXPECT_EQ(buf.data(), res_buffer.data());
    EXPECT_EQ(buf.size(), res_buffer.size());

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(2, 3, 5, 7))(buf));
}

TEST(slice_algorithms_multiply,
     multiplying_two_constants_will_result_in_a_constant)
{
    slice<float> x(2.f);
    slice<float> y(3.f);

    auto res = multiply(x, y, {});

    ASSERT_TRUE(res.is_constant());
    EXPECT_FLOAT_EQ(6.f, res.constant());
}

TEST(slice_algorithms_multiply, multiply_buffer_by_non_zero_one_constant)
{
    alignas(mipp::RequiredAlignment) std::array buf{2.f, 3.f, 5.f, 7.f};
    alignas(mipp::RequiredAlignment) std::array<float, buf.size()> out{};

    auto res = multiply(
            slice<float>(buf),
            slice<float>(3.f),
            std::span<float>(out));

    ASSERT_TRUE(res.is_span());
    EXPECT_EQ(out.data(), res.span().data());
    EXPECT_EQ(out.size(), res.span().size());

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(6.f, 9.f, 15.f, 21.f))(out));
}

TEST(slice_algorithms_multiply, multiply_buffer_by_minus_one_inverts_sign)
{
    alignas(mipp::RequiredAlignment) std::array buf{2.f, -3.f, 5.f, -7.f};
    alignas(mipp::RequiredAlignment) std::array<float, buf.size()> out{};

    auto res = multiply(
            slice<float>(buf),
            slice<float>(-1.f),
            std::span<float>(out));

    ASSERT_TRUE(res.is_span());
    EXPECT_EQ(out.data(), res.span().data());
    EXPECT_EQ(out.size(), res.span().size());

    EXPECT_FLOAT_EQ(-2.f, out[0]);
    EXPECT_FLOAT_EQ(3.f, out[1]);
    EXPECT_FLOAT_EQ(-5.f, out[2]);
    EXPECT_FLOAT_EQ(7.f, out[3]);
}

TEST(slice_algorithms_multiply, multiply_non_zero_one_constant_by_buffer)
{
    alignas(mipp::RequiredAlignment) std::array buf{2.f, 3.f, 5.f, 7.f};
    alignas(mipp::RequiredAlignment) std::array<float, buf.size()> out{};

    auto res = multiply(
            slice<float>(4.f),
            slice<float>(buf),
            std::span<float>(out));

    ASSERT_TRUE(res.is_span());
    EXPECT_EQ(out.data(), res.span().data());
    EXPECT_EQ(out.size(), res.span().size());

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(ElementsAre(8.f, 12.f, 20.f, 28.f))(out));
}

TEST(slice_algorithms_multiply, mulitply_two_buffers)
{
    alignas(mipp::RequiredAlignment)
            std::array buf1{2.f, 3.f, 5.f, 7.f, 4.f, 3.f, 2.f, 1.f};
    alignas(mipp::RequiredAlignment)
            std::array buf2{4.f, 5.f, 6.f, 7.f, 9.f, 9.f, 9.f, 9.f};
    alignas(mipp::RequiredAlignment) std::array<float, buf1.size()> out{};

    auto res = multiply(
            slice<float>(buf1),
            slice<float>(buf2),
            std::span<float>(out));

    ASSERT_TRUE(res.is_span());
    EXPECT_EQ(out.data(), res.span().data());
    EXPECT_EQ(out.size(), res.span().size());

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(
            ElementsAre(8.f, 15.f, 30.f, 49.f, 36.f, 27.f, 18.f, 9.f))(out));
}

TEST(slice_algorithms_subslice, subslicing_constant_returns_just_the_constant)
{
    auto res = subslice(slice<float>(3.f), 3, 5);

    ASSERT_TRUE(res.is_constant());
    EXPECT_FLOAT_EQ(3.f, res.constant());
}

TEST(slice_algorithms_subslice, sublice_buffer)
{
    std::array buf{1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f};

    auto res = subslice(slice<float>(buf), 3, 3);

    ASSERT_TRUE(res.is_span());
    EXPECT_EQ(buf.data() + 3, res.span().data());
    EXPECT_EQ(3u, res.span().size());
}

TEST(slice_interleave_2_slices, both_constant)
{
    slice<float> l(1.f);
    slice<float> r(-1.f);
    std::array<float, 8> out{};
    interleave(l, r, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(
            ElementsAre(1.f, -1.f, 1.f, -1.f, 1.f, -1.f, 1.f, -1.f))(out));
}

TEST(slice_interleave_2_slices, buffer_and_constant)
{
    alignas(mipp::RequiredAlignment)
            std::array buf{1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f};
    slice<float> l(buf);
    slice<float> r(-1.f);
    alignas(mipp::RequiredAlignment) std::array<float, buf.size() * 2> out{};
    interleave(l, r, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            -1.f,
            2.f,
            -1.f,
            3.f,
            -1.f,
            4.f,
            -1.f,
            5.f,
            -1.f,
            6.f,
            -1.f,
            7.f,
            -1.f,
            8.f,
            -1.f))(out));
}

TEST(slice_interleave_2_slices, constant_and_buffer)
{
    alignas(mipp::RequiredAlignment)
            std::array buf{1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f};
    slice<float> l(1.f);
    slice<float> r(buf);
    alignas(mipp::RequiredAlignment) std::array<float, buf.size() * 2> out{};
    interleave(l, r, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            1.f,
            1.f,
            2.f,
            1.f,
            3.f,
            1.f,
            4.f,
            1.f,
            5.f,
            1.f,
            6.f,
            1.f,
            7.f,
            1.f,
            8.f))(out));
}

TEST(slice_interleave_2_slices, two_buffer)
{
    alignas(mipp::RequiredAlignment)
            std::array l_buf{1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f};
    alignas(mipp::RequiredAlignment)
            std::array r_buf{1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f};
    slice<float> l(l_buf);
    slice<float> r(r_buf);
    alignas(mipp::RequiredAlignment) std::array<float, l_buf.size() * 2> out{};
    interleave(l, r, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            1.f,
            2.f,
            2.f,
            3.f,
            3.f,
            4.f,
            4.f,
            5.f,
            5.f,
            6.f,
            6.f,
            7.f,
            7.f,
            8.f,
            8.f))(out));
}

TEST(slice_interleave_4_slices, four_buffer)
{
    alignas(mipp::RequiredAlignment)
            std::array buf1{1.f, 2.f, 3.f, 4.f, 1.f, 1.f, 1.f, 1.f};
    alignas(mipp::RequiredAlignment)
            std::array buf2{5.f, 6.f, 7.f, 8.f, 2.f, 2.f, 2.f, 2.f};
    alignas(mipp::RequiredAlignment)
            std::array buf3{-1.f, -2.f, -3.f, -4.f, 3.f, 3.f, 3.f, 3.f};
    alignas(mipp::RequiredAlignment)
            std::array buf4{-5.f, -6.f, -7.f, -8.f, 4.f, 4.f, 4.f, 4.f};
    slice<float> s1(buf1);
    slice<float> s2(buf2);
    slice<float> s3(buf3);
    slice<float> s4(buf4);
    alignas(mipp::RequiredAlignment) std::array<float, buf1.size() * 4> out{};
    interleave(s1, s2, s3, s4, {out});

    using testing::ElementsAre;
    using testing::Matches;

    EXPECT_TRUE(Matches(ElementsAre(
            1.f,
            5.f,
            -1.f,
            -5.f,
            2.f,
            6.f,
            -2.f,
            -6.f,
            3.f,
            7.f,
            -3.f,
            -7.f,
            4.f,
            8.f,
            -4.f,
            -8.f,
            1.f,
            2.f,
            3.f,
            4.f,
            1.f,
            2.f,
            3.f,
            4.f,
            1.f,
            2.f,
            3.f,
            4.f,
            1.f,
            2.f,
            3.f,
            4.f))(out));
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
    ASSERT_TRUE(res.is_span());
    EXPECT_EQ(buf.size(), res.span().size());
    EXPECT_EQ(buf.data(), res.span().data());

    using testing::ElementsAre;
    using testing::Matches;
    EXPECT_TRUE(Matches(
            ElementsAre(-1.f, 1.f, 0.5f, 1.f, -1.f, 0.23f, 0.f, -0.25f))(buf));
}

} // namespace piejam::audio::engine::test
