// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/processor_map.h>

#include "processor_mock.h"

#include <gtest/gtest.h>

#include <tuple>

namespace piejam::audio::engine::test
{

TEST(processor_map, insert_and_find)
{
    processor_map sut;

    auto proc = std::make_unique<processor_mock>();
    auto const expected = proc.get();

    int const id{5};

    sut.insert(id, std::move(proc));

    int const other_id{6};
    EXPECT_EQ(nullptr, sut.find(other_id));
    EXPECT_EQ(expected, sut.find(id));
}

TEST(processor_map, insert_remove_and_find)
{
    processor_map sut;

    auto proc = std::make_unique<processor_mock>();
    auto const expected = proc.get();

    int const id{5};

    sut.insert(id, std::move(proc));

    ASSERT_EQ(expected, sut.find(id));

    auto removed = sut.remove(id);

    EXPECT_EQ(expected, removed.get());
    EXPECT_EQ(nullptr, sut.find(id));
}

TEST(processor_map, remove_non_existing)
{
    processor_map sut;

    auto removed = sut.remove(int{5});

    EXPECT_EQ(nullptr, removed);
}

} // namespace piejam::audio::engine::test
