// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/ladspa/fwd.h>
#include <piejam/ladspa/instance_manager.h>
#include <piejam/runtime/fx/fwd.h>

#include <map>
#include <memory>
#include <span>

namespace piejam::runtime::fx
{

class ladspa_manager final : public ladspa::instance_manager
{
public:
    ~ladspa_manager();

    auto load(ladspa::plugin_descriptor const&) -> ladspa::instance_id override;
    void unload(ladspa::instance_id const&) override;

    auto control_inputs(ladspa::instance_id const&) const
            -> std::span<ladspa::port_descriptor const> override;

    auto
    make_processor(ladspa::instance_id const&, audio::sample_rate const&) const
            -> std::unique_ptr<audio::engine::processor>;

private:
    std::map<ladspa::instance_id, std::unique_ptr<ladspa::plugin>> m_instances;
};

} // namespace piejam::runtime::fx
