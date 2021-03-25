// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/functional/deref.h>

#include <gtest/gtest.h>

namespace piejam::test
{

TEST(deref, static_asserts)
{
    int* a{};
    int* const b{};
    int const* c{};
    int const* const d{};
    static_assert(std::is_same_v<int&, decltype(deref(a))>);
    static_assert(std::is_same_v<int&, decltype(deref(b))>);
    static_assert(std::is_same_v<int const&, decltype(deref(c))>);
    static_assert(std::is_same_v<int const&, decltype(deref(d))>);

    using p_t = int*;
    static_assert(std::is_same_v<int&, decltype(deref(p_t{}))>);
}

TEST(deref, derefs_a_pointer)
{
    int x{5};
    int* const b = &x;
    int const* const d = &x;

    EXPECT_EQ(5, deref(b));
    EXPECT_EQ(5, deref(d));
}

} // namespace piejam::test
