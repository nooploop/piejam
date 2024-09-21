// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
