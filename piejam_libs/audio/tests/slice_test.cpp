// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/slice.h>

#include <gtest/gtest.h>

#include <array>
#include <span>
#include <vector>

namespace piejam::audio::engine::test
{

TEST(slice, noexcept_construction)
{
    static_assert(std::is_nothrow_default_constructible_v<slice<float>>);
    static_assert(std::is_nothrow_copy_constructible_v<slice<float>>);
    static_assert(std::is_nothrow_copy_assignable_v<slice<float>>);
    static_assert(std::is_nothrow_move_constructible_v<slice<float>>);
    static_assert(std::is_nothrow_move_assignable_v<slice<float>>);
    static_assert(noexcept(slice<float>(5.f)));

    std::array arr{1.f, 2.f, 3.f, 4.f, 5.f};
    static_assert(noexcept(slice<float>(arr)));

    std::vector<float> vec;
    static_assert(noexcept(slice<float>(vec)));

    std::span sp(vec);
    static_assert(noexcept(slice<float>(sp)));
}

} // namespace piejam::audio::engine::test
