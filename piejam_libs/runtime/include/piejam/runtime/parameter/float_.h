// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::runtime::parameter
{

struct float_
{
    using value_type = float;
    using to_normalized_f = float (*)(float_ const&, float);
    using from_normalized_f = float (*)(float_ const&, float);

    float default_value{};

    float min{};
    float max{1.f};

    to_normalized_f to_normalized{[](auto const&, float x) { return x; }};
    from_normalized_f from_normalized{[](auto const&, float x) { return x; }};
};

} // namespace piejam::runtime::parameter
