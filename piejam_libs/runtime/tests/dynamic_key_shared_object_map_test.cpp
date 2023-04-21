// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/dynamic_key_shared_object_map.h>

#include <gtest/gtest.h>

#include <tuple>

namespace piejam::runtime::test
{

TEST(dynamic_key_shared_object_map, insert_and_find)
{
    dynamic_key_shared_object_map<int> sut;

    auto obj = std::make_unique<int>(23);
    auto const expected = obj.get();

    int const id{5};

    sut.insert(id, std::move(obj));

    int const other_id{6};
    EXPECT_EQ(nullptr, sut.find(other_id));
    EXPECT_EQ(expected, sut.find(id).get());
}

} // namespace piejam::runtime::test
