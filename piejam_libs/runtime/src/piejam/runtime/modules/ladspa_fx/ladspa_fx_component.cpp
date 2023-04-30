// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/ladspa_fx/ladspa_fx_component.h>

#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter_processor_factory.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_identity_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/processor_util.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>
#include <boost/container/static_vector.hpp>

#include <algorithm>
#include <array>
#include <vector>

namespace piejam::runtime::modules::ladspa_fx
{

namespace
{

class component final : public audio::engine::component
{
public:
    component(
            fx::module const& fx_mod,
            fx::get_parameter_name const& get_fx_param_name,
            std::unique_ptr<audio::engine::processor> fx_proc,
            parameter_processor_factory& param_proc_factory)
        : m_fx_proc(std::move(fx_proc))
    {
        auto make_graph_endpoint = [this](std::size_t n) {
            return audio::engine::graph_endpoint{*m_fx_proc, n};
        };

        std::ranges::transform(
                range::iota(m_fx_proc->num_inputs()),
                std::back_inserter(m_inputs),
                make_graph_endpoint);

        std::ranges::transform(
                range::iota(m_fx_proc->num_outputs()),
                std::back_inserter(m_outputs),
                make_graph_endpoint);

        BOOST_ASSERT(
                fx_mod.parameters->size() == m_fx_proc->event_inputs().size());

        for (auto&& [key, id] : fx_mod.parameters.get())
        {
            m_param_input_procs.emplace_back(
                    processors::make_parameter_processor(
                            param_proc_factory,
                            id,
                            get_fx_param_name(id)));
        }

        std::ranges::transform(
                range::indices(m_fx_proc->event_inputs()),
                std::back_inserter(m_event_inputs),
                make_graph_endpoint);

        std::ranges::transform(
                range::indices(m_fx_proc->event_outputs()),
                std::back_inserter(m_event_outputs),
                make_graph_endpoint);
    }

    auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    auto outputs() const -> endpoints override
    {
        return m_outputs;
    }

    auto event_inputs() const -> endpoints override
    {
        return m_event_inputs;
    }

    auto event_outputs() const -> endpoints override
    {
        return m_event_outputs;
    }

    void connect(audio::engine::graph& g) const override
    {
        for (std::size_t i : range::indices(m_param_input_procs))
        {
            g.event.insert({*m_param_input_procs[i], 0}, {*m_fx_proc, i});
        }
    }

private:
    std::unique_ptr<audio::engine::processor> m_fx_proc;
    boost::container::static_vector<audio::engine::graph_endpoint, 2> m_inputs;
    boost::container::static_vector<audio::engine::graph_endpoint, 2> m_outputs;
    std::vector<std::shared_ptr<audio::engine::processor>> m_param_input_procs;
    std::vector<audio::engine::graph_endpoint> m_event_inputs;
    std::vector<audio::engine::graph_endpoint> m_event_outputs;
};

class stereo_from_mono_component final : public audio::engine::component
{
public:
    stereo_from_mono_component(
            fx::module const& fx_mod,
            fx::get_parameter_name const& get_fx_param_name,
            std::unique_ptr<audio::engine::processor> fx_left_proc,
            std::unique_ptr<audio::engine::processor> fx_right_proc,
            parameter_processor_factory& param_proc_factory)
        : m_fx_left_proc(std::move(fx_left_proc))
        , m_fx_right_proc(std::move(fx_right_proc))
    {
        BOOST_ASSERT(
                fx_mod.parameters->size() ==
                m_fx_left_proc->event_inputs().size());
        BOOST_ASSERT(
                fx_mod.parameters->size() ==
                m_fx_right_proc->event_inputs().size());

        for (auto&& [key, id] : fx_mod.parameters.get())
        {
            m_param_input_procs.emplace_back(
                    processors::make_parameter_processor(
                            param_proc_factory,
                            id,
                            get_fx_param_name(id)));
        }

        BOOST_ASSERT(
                m_fx_left_proc->event_outputs().size() ==
                m_fx_right_proc->event_outputs().size());
        for (std::size_t n : range::indices(m_fx_left_proc->event_outputs()))
        {
            m_event_outputs.emplace_back(*m_fx_left_proc, n);
            m_event_outputs.emplace_back(*m_fx_right_proc, n);
        }
    }

    auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    auto outputs() const -> endpoints override
    {
        return m_outputs;
    }

    auto event_inputs() const -> endpoints override
    {
        return m_event_inputs;
    }

    auto event_outputs() const -> endpoints override
    {
        return m_event_outputs;
    }

    void connect(audio::engine::graph& g) const override
    {
        for (std::size_t i : range::indices(m_param_input_procs))
        {
            g.event.insert({*m_param_input_procs[i], 0}, {*m_fx_left_proc, i});
            g.event.insert({*m_param_input_procs[i], 0}, {*m_fx_right_proc, i});
        }
    }

private:
    std::unique_ptr<audio::engine::processor> m_fx_left_proc;
    std::unique_ptr<audio::engine::processor> m_fx_right_proc;
    std::array<audio::engine::graph_endpoint, 2> m_inputs{
            {{*m_fx_left_proc, 0}, {*m_fx_right_proc, 0}}};
    std::array<audio::engine::graph_endpoint, 2> m_outputs{
            {{*m_fx_left_proc, 0}, {*m_fx_right_proc, 0}}};
    std::vector<std::shared_ptr<audio::engine::processor>> m_param_input_procs;
    std::vector<audio::engine::graph_endpoint> m_event_inputs;
    std::vector<audio::engine::graph_endpoint> m_event_outputs;
};

} // namespace

auto
make_component(
        fx::module const& fx_mod,
        fx::get_parameter_name const& get_fx_param_name,
        processor_factory const& fx_ladspa_proc_factory,
        parameter_processor_factory& param_proc_factory)
        -> std::unique_ptr<audio::engine::component>
{
    if (auto fx_proc = fx_ladspa_proc_factory(); fx_proc)
    {
        switch (fx_mod.bus_type)
        {
            case audio::bus_type::stereo:
                if (audio::engine::is_stereo_in_out_processor(*fx_proc))
                {
                    return std::make_unique<component>(
                            fx_mod,
                            get_fx_param_name,
                            std::move(fx_proc),
                            param_proc_factory);
                }
                else if (audio::engine::is_mono_in_out_processor(*fx_proc))
                {
                    if (auto fx_second_proc = fx_ladspa_proc_factory();
                        fx_second_proc &&
                        audio::engine::is_mono_in_out_processor(
                                *fx_second_proc))
                    {
                        return std::make_unique<stereo_from_mono_component>(
                                fx_mod,
                                get_fx_param_name,
                                std::move(fx_proc),
                                std::move(fx_second_proc),
                                param_proc_factory);
                    }
                }
                break;

            case audio::bus_type::mono:
                if (audio::engine::is_mono_in_out_processor(*fx_proc))
                {
                    return std::make_unique<component>(
                            fx_mod,
                            get_fx_param_name,
                            std::move(fx_proc),
                            param_proc_factory);
                }
                break;
        }
    }

    return nullptr;
}

} // namespace piejam::runtime::modules::ladspa_fx
