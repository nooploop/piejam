// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/ladspa/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/runtime/fx/fwd.h>

#include <map>
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
    std::map<ladspa_instance_id, std::unique_ptr<audio::ladspa::plugin>>
            m_instances;
};

} // namespace piejam::runtime::fx
