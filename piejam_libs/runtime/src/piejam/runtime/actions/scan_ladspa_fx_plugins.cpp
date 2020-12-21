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

#include <piejam/runtime/actions/scan_ladspa_fx_plugins.h>

#include <piejam/audio/ladspa/scan.h>
#include <piejam/runtime/actions/finalize_ladspa_fx_plugin_scan.h>
#include <piejam/runtime/actions/reload_missing_plugins.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <thread>

namespace piejam::runtime::actions
{

auto
scan_ladspa_fx_plugins(std::filesystem::path const& dir) -> thunk_action
{
    return [dir](auto&&, auto&& dispatch) {
        std::thread([=]() {
            actions::finalize_ladspa_fx_plugin_scan action;
            action.plugins = audio::ladspa::scan_directory(dir);
            dispatch(action);

            dispatch(runtime::actions::reload_missing_plugins());
        }).detach();
    };
}

} // namespace piejam::runtime::actions
