// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/runtime/parameters.h>

#include <boost/container/container_fwd.hpp>

#include <variant>
#include <vector>

namespace piejam::runtime::fx
{

enum class internal : unsigned;

struct ladspa_instance_id_tag;
using ladspa_instance_id = entity_id<ladspa_instance_id_tag>;

struct missing_ladspa;

using instance_id = std::variant<internal, ladspa_instance_id, missing_ladspa>;

class ladspa_manager;

struct parameter;
struct module;
struct registry;

using parameter_id =
        std::variant<float_parameter_id, int_parameter_id, bool_parameter_id>;
using parameter_key = std::size_t;
using module_parameters =
        boost::container::flat_map<parameter_key, parameter_id>;
using parameters_t = boost::container::flat_map<parameter_id, parameter>;

using module_id = entity_id<module>;
using modules_t = entity_map<module, module>;

using chain_t = std::vector<module_id>;

} // namespace piejam::runtime::fx
