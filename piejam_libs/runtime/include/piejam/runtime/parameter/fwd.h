// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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
using id_t = entity_id<Parameter>;

template <template <class> class Value>
class map;

template <class Parameter>
using value_type_t = typename Parameter::value_type;

} // namespace piejam::runtime::parameter
