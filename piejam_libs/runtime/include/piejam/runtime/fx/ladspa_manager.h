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
#include <piejam/audio/ladspa/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/entity_map.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>
#include <span>

namespace piejam::runtime::fx
{

class ladspa_manager
{
public:
    ~ladspa_manager();

    auto load(audio::ladspa::plugin_descriptor const&) -> ladspa_instance_id;
    void unload(ladspa_instance_id const&);

    auto control_inputs(ladspa_instance_id const&) const
            -> std::span<audio::ladspa::port_descriptor const>;

    auto make_processor(ladspa_instance_id const&, audio::samplerate_t) const
            -> std::unique_ptr<audio::engine::processor>;

private:
    entity_map<std::unique_ptr<audio::ladspa::plugin>, ladspa_instance_id_tag>
            m_instances;
};

} // namespace piejam::runtime::fx
