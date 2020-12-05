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

#include <piejam/runtime/actions/finalize_ladspa_fx_plugin_scan.h>

#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/fx/registry.h>

#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

namespace piejam::runtime::actions
{

static void
filter_fx_plugins(std::vector<audio::ladspa::plugin_descriptor>& plugins)
{
    boost::remove_erase_if(
            plugins,
            [](audio::ladspa::plugin_descriptor const& pd) {
                return (pd.num_inputs != pd.num_outputs) ||
                       (pd.num_inputs != 1 && pd.num_inputs != 2);
            });
}

auto
finalize_ladspa_fx_plugin_scan::reduce(state const& st) const -> state
{
    auto new_st = st;

    auto fxs = fx::make_internal_fx_registry_entries();
    auto ladspa_plugins = plugins;
    filter_fx_plugins(ladspa_plugins);
    boost::push_back(fxs, ladspa_plugins);
    new_st.fx_registry.entries = std::move(fxs);

    return new_st;
}

} // namespace piejam::runtime::actions
