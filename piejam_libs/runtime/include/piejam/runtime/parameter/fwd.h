// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/fwd.h>

namespace piejam::runtime::parameter
{

struct bool_descriptor;
struct float_descriptor;
struct int_descriptor;
struct stereo_level_descriptor;

template <class Parameter>
using id_t = entity_id<Parameter>;

template <template <class> class Slot>
class map;

template <class Parameter>
using value_type_t = typename Parameter::value_type;

} // namespace piejam::runtime::parameter
