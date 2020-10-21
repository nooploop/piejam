// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/audio/pcm_convert.h>

#include <gtest/gtest.h>

namespace piejam::audio::pcm_convert::test
{

template <class T>
struct pcm_convert_test : ::testing::Test
{
    using type = T;
};

using pcm_convert_types = ::testing::Types<
        pcm_sample_descriptor_t<pcm_format::s8>,
        pcm_sample_descriptor_t<pcm_format::u8>,
        pcm_sample_descriptor_t<pcm_format::s16_le>,
        pcm_sample_descriptor_t<pcm_format::s16_be>,
        pcm_sample_descriptor_t<pcm_format::u16_le>,
        pcm_sample_descriptor_t<pcm_format::u16_be>,
        pcm_sample_descriptor_t<pcm_format::s32_le>,
        pcm_sample_descriptor_t<pcm_format::s32_be>,
        pcm_sample_descriptor_t<pcm_format::u32_le>,
        pcm_sample_descriptor_t<pcm_format::u32_be>,
        pcm_sample_descriptor_t<pcm_format::s24_3le>,
        pcm_sample_descriptor_t<pcm_format::s24_3be>,
        pcm_sample_descriptor_t<pcm_format::u24_3le>,
        pcm_sample_descriptor_t<pcm_format::u24_3be>>;

TYPED_TEST_CASE(pcm_convert_test, pcm_convert_types);

TYPED_TEST(pcm_convert_test, from_middle)
{
    using desc_t = typename TestFixture::type;
    EXPECT_FLOAT_EQ(
            desc_t::fmiddle,
            from<desc_t::format>(
                    endian_to_format<desc_t::format>(desc_t::middle)));
}

TYPED_TEST(pcm_convert_test, from_min)
{
    using desc_t = typename TestFixture::type;
    EXPECT_FLOAT_EQ(
            desc_t::fmin,
            from<desc_t::format>(
                    endian_to_format<desc_t::format>(desc_t::min)));
}

TYPED_TEST(pcm_convert_test, from_max)
{
    using desc_t = typename TestFixture::type;
    EXPECT_FLOAT_EQ(
            desc_t::fmax,
            from<desc_t::format>(
                    endian_to_format<desc_t::format>(desc_t::max)));
}

TYPED_TEST(pcm_convert_test, to_middle)
{
    using desc_t = typename TestFixture::type;
    EXPECT_EQ(
            endian_to_format<desc_t::format>(desc_t::middle),
            to<desc_t::format>(desc_t::fmiddle));
}

TYPED_TEST(pcm_convert_test, to_min)
{
    using desc_t = typename TestFixture::type;
    EXPECT_EQ(
            endian_to_format<desc_t::format>(desc_t::min),
            to<desc_t::format>(desc_t::fmin));
}

TYPED_TEST(pcm_convert_test, to_max)
{
    using desc_t = typename TestFixture::type;
    EXPECT_EQ(
            endian_to_format<desc_t::format>(desc_t::max),
            to<desc_t::format>(desc_t::fmax));
}

} // namespace piejam::audio::pcm_convert::test
