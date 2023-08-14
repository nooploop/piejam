// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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
struct selected_device;
struct store_dispatch;

using action = ui::action;
using reducible_action = ui::reducible_action<state>;
using get_state_f = ui::get_state_f<state>;
using dispatch_f = ui::dispatch_f;
using next_f = ui::next_f;
using thunk_action = ui::thunk_action<state>;
using batch_action = ui::batch_action;
using update_state_action = ui::update_state_action<state>;

using stereo_level = audio::pair<float>;

struct locations;

using parameter_processor_factory = boost::mp11::
        mp_rename<parameters_t, processors::parameter_processor_factory>;

class fx_parameter_factory;

struct midi_device_config;

struct midi_assignment;
class midi_input_controller;

using middleware_functors = redux::middleware_functors<state, action>;

enum class root_view_mode : int;
enum class fx_browser_add_mode : int;

} // namespace piejam::runtime
