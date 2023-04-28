// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_converter_processor.h>

#include <piejam/audio/engine/processor_test_environment.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <tuple>

namespace piejam::audio::engine::test
{

TEST(event_converter_processor, single_return)
{
    event_converter_processor proc([](float x, float y) { return x + y; });

    processor_test_environment test_env{proc, 16};

    test_env.insert_input_event(0, 0, 77.f);
    test_env.insert_input_event(1, 0, 23.f);

    proc.process(test_env.ctx);

    auto const& ev_out = test_env.event_outputs.get<float>(0);
    ASSERT_EQ(1u, ev_out.size());
    EXPECT_FLOAT_EQ(100.f, ev_out.begin()->value());
}

TEST(event_converter_processor, multiple_returns)
{
    event_converter_processor proc([](float x, float y) {
        auto r = std::minmax(x, y);
        return std::tuple{r.first, r.second};
    });

    processor_test_environment test_env{proc, 16};

    test_env.insert_input_event(0, 0, 58.f);
    test_env.insert_input_event(1, 0, 23.f);

    proc.process(test_env.ctx);

    auto const& ev_out_min = test_env.event_outputs.get<float>(0);
    ASSERT_EQ(1u, ev_out_min.size());
    EXPECT_FLOAT_EQ(23.f, ev_out_min.begin()->value());

    auto const& ev_out_max = test_env.event_outputs.get<float>(1);
    ASSERT_EQ(1u, ev_out_max.size());
    EXPECT_FLOAT_EQ(58.f, ev_out_max.begin()->value());
}

} // namespace piejam::audio::engine::test
