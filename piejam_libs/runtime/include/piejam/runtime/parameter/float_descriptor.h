// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::runtime::parameter
{

struct float_descriptor
{
    using value_type = float;
    using to_normalized_f = float (*)(float_descriptor const&, float);
    using from_normalized_f = float (*)(float_descriptor const&, float);

    float default_value{};

    float min{};
    float max{1.f};

    bool bipolar{};

    to_normalized_f to_normalized{[](auto const&, float x) { return x; }};
    from_normalized_f from_normalized{[](auto const&, float x) { return x; }};
};

} // namespace piejam::runtime::parameter
