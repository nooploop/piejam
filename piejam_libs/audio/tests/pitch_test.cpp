// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/pitch.h>

#include <gtest/gtest.h>

namespace piejam::audio::test
{

struct pitch_test : public testing::TestWithParam<std::pair<float, pitch>>
{
};

TEST_P(pitch_test, basic)
{
    auto [frequency, expected] = GetParam();

    auto p = pitch::from_frequency(frequency);

    EXPECT_EQ(p.pitchclass_, expected.pitchclass_);
    EXPECT_EQ(p.octave, expected.octave);
    EXPECT_NEAR(p.cents, expected.cents, 0.001);
}

INSTANTIATE_TEST_SUITE_P(
        pitch_test_instance,
        pitch_test,
        testing::Values(
                std::pair{440.f, pitch{pitchclass::A, 4, 0.f}},
                std::pair{220.f, pitch{pitchclass::A, 3, 0.f}},
                std::pair{880.f, pitch{pitchclass::A, 5, 0.f}},
                std::pair{110.f, pitch{pitchclass::A, 2, 0.f}},
                std::pair{55.f, pitch{pitchclass::A, 1, 0.f}},
                std::pair{27.5f, pitch{pitchclass::A, 0, 0.f}},
                std::pair{13.75f, pitch{pitchclass::A, -1, 0.f}},
                std::pair{261.6256f, pitch{pitchclass::C, 4, 0.f}},
                std::pair{146.8324f, pitch{pitchclass::D, 3, 0.f}},
                std::pair{441.f, pitch{pitchclass::A, 4, 3.930155f}},
                std::pair{439.f, pitch{pitchclass::A, 4, -3.939097f}},
                std::pair{452.8926f, pitch{pitchclass::A, 4, 49.999f}},
                std::pair{333.f, pitch{pitchclass::E, 4, 17.62229f}},
                std::pair{180.f, pitch{pitchclass::F_sharp, 3, -47.40792f}}));

} // namespace piejam::audio::test
