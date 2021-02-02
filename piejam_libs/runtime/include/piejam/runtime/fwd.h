// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/runtime/parameter/fwd.h>
#include <piejam/runtime/processors/fwd.h>
#include <piejam/runtime/ui/fwd.h>

namespace piejam::runtime
{

class audio_engine;
struct state;
struct selected_device;
struct store_dispatch;

using action = ui::action<state>;
using get_state_f = ui::get_state_f<state>;
using dispatch_f = ui::dispatch_f<state>;
using next_f = ui::next_f<state>;
using thunk_action = ui::thunk_action<state>;
using batch_action = ui::batch_action<state>;

using stereo_level = audio::pair<float>;

struct locations;

using parameter_processor_factory = processors::parameter_processor_factory<
        parameter::float_,
        parameter::int_,
        parameter::bool_,
        parameter::stereo_level>;

struct midi_device_config;

struct midi_assignment;

} // namespace piejam::runtime
