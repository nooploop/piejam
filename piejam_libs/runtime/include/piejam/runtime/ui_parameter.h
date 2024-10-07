// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/parameters.h>

#include <piejam/entity_id.h>

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
