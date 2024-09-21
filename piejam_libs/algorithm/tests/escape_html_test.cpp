// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/algorithm/escape_html.h>

#include <gtest/gtest.h>

#include <string>

namespace piejam::algorithm::test
{

TEST(escape_html, none)
{
    EXPECT_EQ("abc", escape_html("abc"));
}

TEST(escape_html, all)
{
    EXPECT_EQ("a&amp;b&apos;c&quot;d&lt;e&gt;f", escape_html("a&b'c\"d<e>f"));
}

} // namespace piejam::algorithm::test
