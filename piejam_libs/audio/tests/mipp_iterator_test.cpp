// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/dsp/mipp_iterator.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace piejam::audio::dsp::test
{

TEST(mipp_iterator_test, write)
{
    mipp::vector<float> buffer{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};

    auto it = mipp_iterator{buffer.data()};

    mipp::Reg<float> reg = *it;

    *it = reg * 2;

    EXPECT_THAT(
            buffer,
            testing::ElementsAre(
                    2.0f,
                    4.0f,
                    6.0f,
                    8.0f,
                    5.0f,
                    6.0f,
                    7.0f,
                    8.0f));
}

TEST(mipp_iterator_test, concepts)
{
    static_assert(std::input_iterator<mipp_iterator<float const>>);
    static_assert(std::output_iterator<mipp_iterator<float>, mipp::Reg<float>>);
}

} // namespace piejam::audio::dsp::test
