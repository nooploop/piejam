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

#include <piejam/runtime/fx/ladspa_manager.h>

#include <piejam/audio/ladspa/plugin.h>

#include <spdlog/spdlog.h>

namespace piejam::runtime::fx
{

ladspa_manager::~ladspa_manager() = default;

auto
ladspa_manager::load(audio::ladspa::plugin_descriptor const& pd)
        -> ladspa_instance_id
{
    try
    {
        auto plugin = audio::ladspa::load(pd);
        return m_instances.add(std::move(plugin));
    }
    catch (std::exception const& err)
    {
        spdlog::error("Loading LADSPA fx plugin failed: {}", err.what());
        return {};
    }
}

void
ladspa_manager::unload(ladspa_instance_id const& id)
{
    m_instances.remove(id);
}

} // namespace piejam::runtime::fx
