// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/range/indices.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>

namespace piejam::range::test
{

TEST(indices, basic)
{
    std::array arr{1.f, 2.f, 3.f, 4.f, 5.f};
    std::vector<std::size_t> expected{0u, 1u, 2u, 3u, 4u};

    auto idxs = indices(arr);
    EXPECT_TRUE(std::ranges::equal(idxs, expected));
}

} // namespace piejam::range::test
