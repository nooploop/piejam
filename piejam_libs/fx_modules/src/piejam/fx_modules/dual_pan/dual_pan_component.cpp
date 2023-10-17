// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/dual_pan/dual_pan_component.h>

#include <piejam/audio/components/pan_balance.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/mix_processor.h>
#include <piejam/fx_modules/dual_pan/dual_pan_module.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/internal_fx_component_factory.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

#include <array>

namespace piejam::fx_modules::dual_pan
{

namespace
{

class component final : public audio::engine::component
{
public:
    component(
            runtime::fx::module const& fx_mod,
            runtime::parameter_processor_factory& proc_factory,
            std::string_view const name)
        : m_left_param_proc(runtime::processors::make_parameter_processor(
                  proc_factory,
                  fx_mod.parameters->at(to_underlying(parameter_key::left_pan)),
                  fmt::format("left_pan {}", name)))
        , m_right_param_proc(runtime::processors::make_parameter_processor(
                  proc_factory,
                  fx_mod.parameters->at(
                          to_underlying(parameter_key::right_pan)),
                  fmt::format("right_pan {}", name)))

    {
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
        return {};
    }

    void connect(audio::engine::graph& g) const override
    {
        m_left_pan->connect(g);
        m_right_pan->connect(g);

        using namespace audio;
        engine::connect_event(
                g,
                *m_left_param_proc,
                engine::endpoint_ports::from<0>{},
                *m_left_pan,
                engine::endpoint_ports::to<0>{});
        engine::connect_event(
                g,
                *m_right_param_proc,
                engine::endpoint_ports::from<0>{},
                *m_right_pan,
                engine::endpoint_ports::to<0>{});

        engine::connect(
                g,
                *m_left_pan,
                engine::endpoint_ports::from<0>{},
                *m_left_mix_proc,
                engine::endpoint_ports::to<0>{});
        engine::connect(
                g,
                *m_right_pan,
                engine::endpoint_ports::from<0>{},
                *m_left_mix_proc,
                engine::endpoint_ports::to<1>{});

        engine::connect(
                g,
                *m_left_pan,
                engine::endpoint_ports::from<1>{},
                *m_right_mix_proc,
                engine::endpoint_ports::to<0>{});
        engine::connect(
                g,
                *m_right_pan,
                engine::endpoint_ports::from<1>{},
                *m_right_mix_proc,
                engine::endpoint_ports::to<1>{});
    }

private:
    std::shared_ptr<audio::engine::processor> m_left_param_proc;
    std::shared_ptr<audio::engine::processor> m_right_param_proc;
    std::array<audio::engine::graph_endpoint, 2> const m_event_inputs{
            audio::engine::graph_endpoint{
                    .proc = *m_left_param_proc,
                    .port = 0},
            audio::engine::graph_endpoint{
                    .proc = *m_right_param_proc,
                    .port = 0},
    };
    std::unique_ptr<audio::engine::component> m_left_pan{
            audio::components::make_pan("left_pan")};
    std::unique_ptr<audio::engine::component> m_right_pan{
            audio::components::make_pan("right_pan")};
    std::array<audio::engine::graph_endpoint, 2> const m_inputs{
            audio::engine::in_endpoint(*m_left_pan, 0),
            audio::engine::in_endpoint(*m_right_pan, 0)};
    std::unique_ptr<audio::engine::processor> m_left_mix_proc{
            audio::engine::make_mix_processor(2, "left_pan")};
    std::unique_ptr<audio::engine::processor> m_right_mix_proc{
            audio::engine::make_mix_processor(2, "right_pan")};
    std::array<audio::engine::graph_endpoint, 2> const m_outputs{
            audio::engine::graph_endpoint{.proc = *m_left_mix_proc, .port = 0},
            audio::engine::graph_endpoint{.proc = *m_right_mix_proc, .port = 0},
    };
};

} // namespace

auto
make_component(runtime::internal_fx_component_factory_args const& args)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<component>(
            args.fx_mod,
            args.param_procs,
            args.name);
}

} // namespace piejam::fx_modules::dual_pan
