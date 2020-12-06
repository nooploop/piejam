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

#include <piejam/audio/engine/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <functional>
#include <memory>
#include <string_view>

namespace piejam::runtime::components
{

using fx_ladspa_processor_factory =
        std::function<std::unique_ptr<audio::engine::processor>()>;

auto make_fx_ladspa(
        fx::module const&,
        fx_ladspa_processor_factory,
        parameter_processor_factory&)
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
