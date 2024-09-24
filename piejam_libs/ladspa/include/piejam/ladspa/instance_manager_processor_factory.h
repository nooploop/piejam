// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>
#include <piejam/ladspa/fwd.h>
#include <piejam/ladspa/instance_manager.h>
#include <piejam/ladspa/processor_factory.h>

#include <map>
#include <memory>
#include <span>

namespace piejam::ladspa
{

class instance_manager_processor_factory final
    : public instance_manager
    , public processor_factory
{
public:
    ~instance_manager_processor_factory() override;

    auto load(plugin_descriptor const&) -> instance_id override;
    void unload(instance_id const&) override;

    [[nodiscard]]
    auto control_inputs(instance_id const&) const
            -> std::span<port_descriptor const> override;

    auto make_processor(instance_id const&, audio::sample_rate)
            -> std::unique_ptr<audio::engine::processor> override;

private:
    std::map<instance_id, std::unique_ptr<plugin>> m_instances;
};

} // namespace piejam::ladspa
