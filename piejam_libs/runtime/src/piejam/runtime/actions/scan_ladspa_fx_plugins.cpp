// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
