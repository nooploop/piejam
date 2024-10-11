// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/utility/utility_component.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/fx_modules/utility/utility_module.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/internal_fx_component_factory.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

#include <array>

namespace piejam::fx_modules::utility
{

namespace
{

auto
mono_gain_converter(float gain, bool invert) -> float
{
    return invert ? -gain : gain;
}

auto
stereo_gain_converter(float gain, bool invert_left, bool invert_right)
        -> std::tuple<float, float>
{
    return std::tuple{invert_left ? -gain : gain, invert_right ? -gain : gain};
}

auto
make_gain_converter_processor(audio::bus_type bus_type, std::string_view name)
{
    using namespace std::string_view_literals;
    return bus_type == audio::bus_type::mono
                   ? audio::engine::make_event_converter_processor(
                             &mono_gain_converter,
                             std::array{"gain"sv, "invert"sv},
                             std::array{"gain"sv},
                             name)
                   : audio::engine::make_event_converter_processor(
                             &stereo_gain_converter,
                             std::array{
                                     "gain"sv,
                                     "invert_left"sv,
                                     "invert_right"sv},
                             std::array{"gain_left"sv, "gain_right"sv},
                             name);
}

class component final : public audio::engine::component
{
public:
    component(
            runtime::fx::module const& fx_mod,
            runtime::parameter_processor_factory& proc_factory,
            std::string_view const name)
        : m_gain_param_proc{runtime::processors::
                                    find_or_make_parameter_processor(
                                            proc_factory,
                                            fx_mod.parameters->at(to_underlying(
                                                    parameter_key::gain)),
                                            fmt::format(
                                                    "utility_gain {}",
                                                    name))}
        , m_invert_param_proc{fx_mod.bus_type == audio::bus_type::mono ? runtime::processors::
                                   find_or_make_parameter_processor(
                                           proc_factory,
                                           fx_mod.parameters->at(to_underlying(
                                                   parameter_key::invert)),
                                           fmt::format(
                                                   "utility_invert {}",
                                                   name)) : nullptr}
        , m_invert_left_param_proc{fx_mod.bus_type == audio::bus_type::stereo ? runtime::processors::
                                      find_or_make_parameter_processor(
                                              proc_factory,
                                              fx_mod.parameters->at(to_underlying(
                                                      parameter_key::invert_left)),
                                              fmt::format(
                                                      "utility_invert {}",
                                                      name)) : nullptr}
        , m_invert_right_param_proc{fx_mod.bus_type == audio::bus_type::stereo ? runtime::processors::
                                      find_or_make_parameter_processor(
                                              proc_factory,
                                              fx_mod.parameters->at(to_underlying(
                                                      parameter_key::invert_right)),
                                              fmt::format(
                                                      "utility_invert {}",
                                                      name)) : nullptr}
        , m_gain_converter_proc{make_gain_converter_processor(fx_mod.bus_type, name)}
        , m_amplifier{
                  fx_mod.bus_type == audio::bus_type::mono
                          ? audio::components::make_mono_amplifier(
                                    fmt::format("utility {}", name))
                          : audio::components::make_stereo_split_amplifier(
                                    fmt::format("utility {}", name))}
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_amplifier->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_amplifier->outputs();
    }

    auto event_inputs() const -> endpoints override
    {
        return {};
    }

    auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(audio::engine::graph& g) const override
    {
        using namespace audio::engine::endpoint_ports;

        m_amplifier->connect(g);

        audio::engine::connect_event(
                g,
                *m_gain_param_proc,
                from<0>,
                *m_gain_converter_proc,
                to<0>);

        if (m_invert_param_proc)
        {
            audio::engine::connect_event(
                    g,
                    *m_invert_param_proc,
                    from<0>,
                    *m_gain_converter_proc,
                    to<1>);

            audio::engine::connect_event(
                    g,
                    *m_gain_converter_proc,
                    from<0>,
                    *m_amplifier,
                    to<0>);
        }
        else
        {
            BOOST_ASSERT(m_invert_left_param_proc);
            BOOST_ASSERT(m_invert_right_param_proc);

            audio::engine::connect_event(
                    g,
                    *m_invert_left_param_proc,
                    from<0>,
                    *m_gain_converter_proc,
                    to<1>);

            audio::engine::connect_event(
                    g,
                    *m_invert_right_param_proc,
                    from<0>,
                    *m_gain_converter_proc,
                    to<2>);

            audio::engine::connect_event(
                    g,
                    *m_gain_converter_proc,
                    from<0, 1>,
                    *m_amplifier,
                    to<0, 1>);
        }
    }

private:
    std::shared_ptr<audio::engine::processor> m_gain_param_proc;
    std::shared_ptr<audio::engine::processor> m_invert_param_proc;
    std::shared_ptr<audio::engine::processor> m_invert_left_param_proc;
    std::shared_ptr<audio::engine::processor> m_invert_right_param_proc;
    std::unique_ptr<audio::engine::processor> m_gain_converter_proc;
    std::unique_ptr<audio::engine::component> m_amplifier;
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

} // namespace piejam::fx_modules::utility
