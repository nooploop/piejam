// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/fx_ladspa.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_identity_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/range/indices.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter_processor_factory.h>

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <vector>

namespace piejam::runtime::components
{

namespace
{

class fx_ladspa final : public audio::engine::component
{
public:
    fx_ladspa(
            fx::module const& fx_mod,
            fx::get_parameter_name const& get_fx_param_name,
            std::unique_ptr<audio::engine::processor> fx_proc,
            parameter_processor_factory& param_proc_factory)
        : m_fx_proc(std::move(fx_proc))
    {
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
                [this](std::size_t n) {
                    return audio::engine::graph_endpoint{*m_fx_proc, n};
                });

        std::ranges::transform(
                range::indices(m_fx_proc->event_outputs()),
                std::back_inserter(m_event_outputs),
                [this](std::size_t n) {
                    return audio::engine::graph_endpoint{*m_fx_proc, n};
                });
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return m_event_outputs; }

    void connect(audio::engine::graph& g) const override
    {
        for (std::size_t i : range::indices(m_param_input_procs))
        {
            g.add_event_wire({*m_param_input_procs[i], 0}, {*m_fx_proc, i});
        }
    }

private:
    std::unique_ptr<audio::engine::processor> m_fx_proc;
    std::array<audio::engine::graph_endpoint, 2> m_inputs{
            {{*m_fx_proc, 0}, {*m_fx_proc, 1}}};
    std::array<audio::engine::graph_endpoint, 2> m_outputs{
            {{*m_fx_proc, 0}, {*m_fx_proc, 1}}};
    std::vector<std::shared_ptr<audio::engine::processor>> m_param_input_procs;
    std::vector<audio::engine::graph_endpoint> m_event_inputs;
    std::vector<audio::engine::graph_endpoint> m_event_outputs;
};

template <class P>
auto
make_event_identity_processor(parameter::id_t<P> const&)
{
    return audio::engine::make_event_identity_processor<
            typename P::value_type>();
}

auto
make_event_identity_processor(fx::parameter_id const& id)
{
    return std::visit(
            [](auto&& param_id) {
                return make_event_identity_processor(param_id);
            },
            id);
}

class fx_ladspa_from_mono final : public audio::engine::component
{
public:
    fx_ladspa_from_mono(
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

            m_input_event_identity_procs.emplace_back(
                    make_event_identity_processor(id));
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

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return m_event_outputs; }

    void connect(audio::engine::graph& g) const override
    {
        for (std::size_t i : range::indices(m_param_input_procs))
        {
            g.add_event_wire(
                    {*m_param_input_procs[i], 0},
                    {*m_input_event_identity_procs[i], 0});
            g.add_event_wire(
                    {*m_input_event_identity_procs[i], 0},
                    {*m_fx_left_proc, i});
            g.add_event_wire(
                    {*m_input_event_identity_procs[i], 0},
                    {*m_fx_right_proc, i});
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
    std::vector<std::unique_ptr<audio::engine::processor>>
            m_input_event_identity_procs;
    std::vector<audio::engine::graph_endpoint> m_event_inputs;
    std::vector<audio::engine::graph_endpoint> m_event_outputs;
};

} // namespace

auto
make_fx_ladspa(
        fx::module const& fx_mod,
        fx::get_parameter_name const& get_fx_param_name,
        fx_ladspa_processor_factory const& fx_ladspa_proc_factory,
        parameter_processor_factory& param_proc_factory)
        -> std::unique_ptr<audio::engine::component>
{
    auto fx_proc = fx_ladspa_proc_factory();
    if (fx_proc && fx_proc->num_inputs() == 2 && fx_proc->num_outputs() == 2)
    {
        return std::make_unique<fx_ladspa>(
                fx_mod,
                get_fx_param_name,
                std::move(fx_proc),
                param_proc_factory);
    }
    else if (
            fx_proc && fx_proc->num_inputs() == 1 &&
            fx_proc->num_outputs() == 1)
    {
        if (auto fx_second_proc = fx_ladspa_proc_factory())
        {
            return std::make_unique<fx_ladspa_from_mono>(
                    fx_mod,
                    get_fx_param_name,
                    std::move(fx_proc),
                    std::move(fx_second_proc),
                    param_proc_factory);
        }
    }

    return nullptr;
}

} // namespace piejam::runtime::components
