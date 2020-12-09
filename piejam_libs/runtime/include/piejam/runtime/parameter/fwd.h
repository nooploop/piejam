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

#include <piejam/audio/fwd.h>
#include <piejam/fwd.h>

namespace piejam::runtime::parameter
{

template <class Value>
struct generic_value;

using bool_ = generic_value<bool>;
struct float_;
struct int_;
using stereo_level = generic_value<audio::pair<float>>;

template <class Parameter>
class map;

template <class... Parameter>
class maps_collection;

template <class Parameter>
using id_t = entity_id<Parameter>;

} // namespace piejam::runtime::parameter
