// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_converter_processor.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event_converter_processor, int_to_float)
{
    event_converter_processor sut([](int x) -> float { return x; });
}

} // namespace piejam::audio::engine::test
