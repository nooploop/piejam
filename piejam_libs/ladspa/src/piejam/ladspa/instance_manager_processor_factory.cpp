// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/ladspa/instance_manager_processor_factory.h>

#include <piejam/audio/engine/processor.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/entity_id_hash.h>
#include <piejam/ladspa/plugin.h>
#include <piejam/ladspa/plugin_descriptor.h>

#include <spdlog/spdlog.h>

namespace piejam::ladspa
{

instance_manager_processor_factory::~instance_manager_processor_factory() =
        default;

auto
instance_manager_processor_factory::load(plugin_descriptor const& pd)
        -> instance_id
{
    try
    {
        auto plugin = ladspa::load(pd);
        auto id = entity_id<instance_id_tag>::generate();
        m_instances.emplace(id, std::move(plugin));
        return id;
    }
    catch (std::exception const& err)
    {
        spdlog::error("Loading LADSPA fx plugin failed: {}", err.what());
        return {};
    }
}

void
instance_manager_processor_factory::unload(instance_id const& id)
{
    m_instances.erase(id);
}

auto
instance_manager_processor_factory::control_inputs(instance_id const& id) const
        -> std::span<port_descriptor const>
{
    if (auto it = m_instances.find(id); it != m_instances.end())
    {
        return it->second->control_inputs();
    }

    return {};
}

auto
instance_manager_processor_factory::make_processor(
        instance_id const& id,
        audio::sample_rate const& sample_rate)
        -> std::unique_ptr<audio::engine::processor>
{
    if (auto it = m_instances.find(id); it != m_instances.end())
    {
        return it->second->make_processor(sample_rate);
    }

    return {};
}

} // namespace piejam::ladspa
