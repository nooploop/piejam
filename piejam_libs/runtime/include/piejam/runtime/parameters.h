// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/fwd.h>

#include <piejam/audio/fwd.h>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/map.hpp>

#include <variant>

namespace piejam::runtime
{

using float_parameter = parameter::float_descriptor;
using float_parameter_id = parameter::id_t<float_parameter>;

using bool_parameter = parameter::bool_descriptor;
using bool_parameter_id = parameter::id_t<bool_parameter>;

using int_parameter = parameter::int_descriptor;
using int_parameter_id = parameter::id_t<int_parameter>;

using stereo_level_parameter = parameter::stereo_level_descriptor;
using stereo_level_parameter_id = parameter::id_t<stereo_level_parameter>;

// Store the value type here, so we don't have to include the descriptor
// headers.
using parameters_t = boost::mp11::mp_list<
        boost::mp11::mp_list<bool_parameter, bool>,
        boost::mp11::mp_list<float_parameter, float>,
        boost::mp11::mp_list<int_parameter, int>,
        boost::mp11::mp_list<stereo_level_parameter, audio::pair<float>>>;

using parameter_ids_t = boost::mp11::mp_map_keys<parameters_t>;

template <class Parameter>
using parameter_value_type_t = boost::mp11::mp_second<
        boost::mp11::mp_map_find<parameters_t, Parameter>>;

using parameter_id = boost::mp11::mp_rename<
        boost::mp11::mp_transform<parameter::id_t, parameter_ids_t>,
        std::variant>;

using parameter_value = boost::mp11::mp_rename<
        boost::mp11::mp_transform<boost::mp11::mp_second, parameters_t>,
        std::variant>;

template <class ParamId>
using is_persistable_parameter = boost::mp11::mp_contains<
        boost::mp11::mp_list<
                bool_parameter_id,
                float_parameter_id,
                int_parameter_id>,
        ParamId>;

template <class ParamId>
constexpr bool is_persistable_parameter_v =
        is_persistable_parameter<ParamId>::value;

} // namespace piejam::runtime
