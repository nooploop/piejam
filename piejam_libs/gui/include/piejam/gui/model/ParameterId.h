// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/parameters.h>

#include <variant>

namespace piejam::gui::model
{

using ParameterId = std::variant<
        runtime::bool_parameter_id,
        runtime::float_parameter_id,
        runtime::int_parameter_id>;

} // namespace piejam::gui::model
