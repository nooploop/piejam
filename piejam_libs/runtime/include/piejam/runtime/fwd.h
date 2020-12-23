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
        std::tuple<parameter::float_, parameter::int_, parameter::bool_>,
        std::tuple<parameter::stereo_level>>;

} // namespace piejam::runtime
