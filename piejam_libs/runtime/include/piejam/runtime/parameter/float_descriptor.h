// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>

#include <fmt/format.h>

namespace piejam::runtime::parameter
{

struct float_descriptor
{
    using value_type = float;
    using value_to_string_fn = std::string (*)(value_type);
    using to_normalized_f = float (*)(float_descriptor const&, float);
    using from_normalized_f = float (*)(float_descriptor const&, float);

    boxed_string name;

    float default_value;

    float min;
    float max;

    bool bipolar{};

    value_to_string_fn value_to_string{
            [](value_type x) { return fmt::format("{:.2f}", x); }};

    to_normalized_f to_normalized{[](auto const&, float x) { return x; }};
    from_normalized_f from_normalized{[](auto const&, float x) { return x; }};
};

} // namespace piejam::runtime::parameter
