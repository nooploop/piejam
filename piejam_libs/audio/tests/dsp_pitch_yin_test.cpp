// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/dsp/pitch_yin.h>

#include <piejam/audio/dsp/generate_sine.h>

#include <mipp.h>

#include <gtest/gtest.h>

namespace piejam::audio::dsp::test
{

// test param: frequency
struct pitch_yin_test : public testing::TestWithParam<float>
{
    static constexpr std::size_t buffer_size{8192};
    static constexpr sample_rate sr{48000};

    pitch_yin_test()
        : signal(buffer_size)
    {
        float const freq = GetParam();
        generate_sine(std::span{signal}, sr.as_float(), freq);
    }

    mipp::vector<float> signal;
};

TEST_P(pitch_yin_test, simple)
{
    auto result = pitch_yin<float>(signal, sr);

    EXPECT_NEAR(GetParam(), result, 0.5);
}

static auto s_test_frequencies = testing::Values(
        27.5f,
        30.87f,
        32.7f,
        36.71f,
        41.2f,
        43.65f,
        49.f,
        55.f,
        61.74f,
        65.41f,
        73.42f,
        82.41f,
        87.31f,
        98.f,
        110.f,
        123.47f,
        130.81f,
        146.83f,
        164.81f,
        174.61f,
        196.f,
        220.f,
        246.94f,
        261.63f,
        293.66f,
        329.63f,
        349.23f,
        392.f,
        440.f,
        493.88f,
        523.25f,
        587.33f,
        659.26f,
        698.46f,
        783.99f,
        880.f,
        987.77f,
        1046.5f,
        1174.66f,
        1318.51f,
        1396.91f,
        1567.98f);

INSTANTIATE_TEST_SUITE_P(all, pitch_yin_test, s_test_frequencies);

} // namespace piejam::audio::dsp::test
