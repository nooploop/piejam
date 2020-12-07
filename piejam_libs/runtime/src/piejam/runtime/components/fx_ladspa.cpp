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

#include <piejam/runtime/components/fx_ladspa.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/value_input_processor.h>
#include <piejam/range/indices.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter_processor_factory.h>

#include <fmt/format.h>

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
            std::unique_ptr<audio::engine::processor> fx_proc,
            parameter_processor_factory& param_proc_factory)
        : m_fx_proc(std::move(fx_proc))
    {
        BOOST_ASSERT(
                fx_mod.parameters->size() == m_fx_proc->event_inputs().size());
        BOOST_ASSERT_MSG(
                m_fx_proc->event_outputs().empty(),
                "ladspa output control not implemented");

        for (auto&& [key, id] : fx_mod.parameters.get())
        {
            m_param_input_procs.emplace_back(
                    processors::make_input_processor(param_proc_factory, id));
        }
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return {}; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) const override
    {
        for (std::size_t i : range::indices(m_param_input_procs.size()))
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
};

class fx_ladspa_from_mono final : public audio::engine::component
{
public:
    fx_ladspa_from_mono(
            fx::module const& fx_mod,
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
        BOOST_ASSERT_MSG(
                m_fx_left_proc->event_outputs().empty(),
                "ladspa output control not implemented");
        BOOST_ASSERT_MSG(
                m_fx_right_proc->event_outputs().empty(),
                "ladspa output control not implemented");

        for (auto&& [key, id] : fx_mod.parameters.get())
        {
            m_param_input_procs.emplace_back(
                    processors::make_input_processor(param_proc_factory, id));
        }
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return {}; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) const override
    {
        for (std::size_t i : range::indices(m_param_input_procs.size()))
        {
            g.add_event_wire(
                    {*m_param_input_procs[i], 0},
                    {*m_fx_left_proc, i});
            g.add_event_wire(
                    {*m_param_input_procs[i], 0},
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
};

} // namespace

auto
make_fx_ladspa(
        fx::module const& fx_mod,
        fx_ladspa_processor_factory const& fx_ladspa_proc_factory,
        parameter_processor_factory& param_proc_factory)
        -> std::unique_ptr<audio::engine::component>
{
    auto fx_proc = fx_ladspa_proc_factory();
    if (fx_proc && fx_proc->num_inputs() == 2 && fx_proc->num_outputs() == 2)
    {
        return std::make_unique<fx_ladspa>(
                fx_mod,
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
                    std::move(fx_proc),
                    std::move(fx_second_proc),
                    param_proc_factory);
        }
    }

    return nullptr;
}

} // namespace piejam::runtime::components
