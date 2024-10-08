// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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

    value_to_string_fn value_to_string{};
};

using ui_parameters_map = parameter::map<ui_parameter_map_slot>;

} // namespace piejam::runtime
