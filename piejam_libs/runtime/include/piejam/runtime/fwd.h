// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/redux/fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/processors/fwd.h>
#include <piejam/runtime/ui/fwd.h>

#include <boost/mp11/list.hpp>

namespace piejam::runtime
{

class audio_engine;
struct state;
struct selected_sound_card;
struct store_dispatch;

using action = ui::action;
using reducible_action = ui::reducible_action<state>;
using get_state_f = ui::get_state_f<state>;
using dispatch_f = ui::dispatch_f;
using next_f = ui::next_f;
using thunk_action = ui::thunk_action<state>;
using update_state_action = ui::update_state_action<state>;

struct locations;

template <class Parameter>
struct parameter_map_slot;
using parameters_map = parameter::map<parameter_map_slot>;

using parameter_processor_factory = boost::mp11::
        mp_rename<parameter_ids_t, processors::parameter_processor_factory>;

template <template <class> class... Value>
class parameter_factory;

struct midi_device_config;

struct midi_assignment;
class midi_input_controller;

using middleware_functors = redux::middleware_functors<state, action>;

enum class root_view_mode : int;
enum class fx_browser_add_mode : int;

struct internal_fx_component_factory_args;

} // namespace piejam::runtime
