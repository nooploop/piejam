// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/flag_resetter.h>

#include <gtest/gtest.h>

namespace piejam::redux::test
{

TEST(flag_resetter, functionality)
{
    bool flag{true};

    {
        flag_resetter reset{&flag};
        EXPECT_TRUE(flag);
    }

    EXPECT_FALSE(flag);
}

} // namespace piejam::redux::test
