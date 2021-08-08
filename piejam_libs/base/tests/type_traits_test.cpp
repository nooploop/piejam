// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/type_traits.h>

#include <gtest/gtest.h>

namespace piejam::test
{

struct default_ctor_except
{
    default_ctor_except() noexcept(false) {}
};

TEST(type_traits, is_nothrow_default_constructible)
{
    static_assert(is_nothrow_default_constructible_v<float>);
    static_assert(!is_nothrow_default_constructible_v<default_ctor_except>);
    static_assert(is_nothrow_default_constructible_v<float, int, char>);
    static_assert(!is_nothrow_default_constructible_v<
                  float,
                  int,
                  char,
                  default_ctor_except>);
}

} // namespace piejam::test
