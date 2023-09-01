// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameters.h>

#include <boost/mp11/algorithm.hpp>

#include <variant>

namespace piejam::runtime
{

using ui_parameter_id =
        std::variant<float_parameter_id, int_parameter_id, bool_parameter_id>;

template <class Id>
using parameter_id_to_value_type = typename Id::tag::value_type;

using ui_parameter_value =
        boost::mp11::mp_transform<parameter_id_to_value_type, ui_parameter_id>;

} // namespace piejam::runtime
