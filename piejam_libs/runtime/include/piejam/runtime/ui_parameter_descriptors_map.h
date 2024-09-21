// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>
#include <piejam/runtime/parameter/fwd.h>

namespace piejam::runtime
{

template <class Parameter>
struct ui_parameter_map_slot
{
    using value_to_string_fn =
            std::string (*)(parameter::value_type_t<Parameter>);

    boxed_string name;
    value_to_string_fn value_to_string{};
    bool bipolar{};
};

using ui_parameters_map = parameter::map<ui_parameter_map_slot>;

} // namespace piejam::runtime
