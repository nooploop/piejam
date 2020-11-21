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

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/fwd.h>

namespace piejam::runtime
{

class audio_engine;
struct audio_state;
struct selected_device;

using action = ui::action<audio_state>;
using get_state_f = ui::get_state_f<audio_state>;
using dispatch_f = ui::dispatch_f<audio_state>;
using thunk_f = ui::thunk_f<audio_state>;
using thunk_action = ui::thunk_action<audio_state>;

template <class Parameter>
class parameter_map;

} // namespace piejam::runtime
