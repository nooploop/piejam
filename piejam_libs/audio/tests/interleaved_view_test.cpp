// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/interleaved_view.h>

#include <gtest/gtest.h>

#include <array>

namespace piejam::audio::test
{

TEST(interleaved_view, concept_check)
{
    static_assert(std::ranges::random_access_range<interleaved_view<float, 2>>);
}

TEST(interleaved_view, begin_end_for_static)
{
    std::array<float, 4> buf{1.f, 2.f, 3.f, 4.f};
    interleaved_view<float, 2> v(buf.data(), 2);

    auto it_begin = std::ranges::begin(v);
    EXPECT_EQ(it_begin->data(), buf.data());

    auto it_end = std::ranges::end(v);
    EXPECT_EQ(it_end->data(), buf.data() + buf.size());
}

} // namespace piejam::audio::test
