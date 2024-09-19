// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/finalize_ladspa_fx_plugin_scan.h>

#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/state.h>

#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

namespace piejam::runtime::actions
{

static void
filter_fx_plugins(std::vector<ladspa::plugin_descriptor>& plugins)
{
    boost::remove_erase_if(plugins, [](ladspa::plugin_descriptor const& pd) {
        return (pd.num_inputs != pd.num_outputs) ||
               (pd.num_inputs != 1 && pd.num_inputs != 2);
    });
}

static void
sort_fx_plugins(std::vector<ladspa::plugin_descriptor>& plugins)
{
    std::ranges::sort(plugins, {}, &ladspa::plugin_descriptor::name);
}

void
finalize_ladspa_fx_plugin_scan::reduce(state& st) const
{
    auto ladspa_plugins = plugins;
    filter_fx_plugins(ladspa_plugins);
    sort_fx_plugins(ladspa_plugins);

    fx::registry new_registry;
    {
        auto fxs = new_registry.entries.lock();
        boost::push_back(*fxs, std::move(ladspa_plugins));
    }

    st.fx_registry = std::move(new_registry);
}

} // namespace piejam::runtime::actions
