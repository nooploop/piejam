// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/fwd.h>

#include <boost/container/container_fwd.hpp>

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

template <class Parameter>
using value_type_t = typename Parameter::value_type;

template <class Parameter>
using id_value_map_t =
        boost::container::flat_map<id_t<Parameter>, value_type_t<Parameter>>;

} // namespace piejam::runtime::parameter
