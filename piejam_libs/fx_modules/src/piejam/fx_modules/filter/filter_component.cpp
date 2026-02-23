// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "filter_component.h"

#include "filter_module.h"

#include <piejam/audio/components/biquad_filter.h>
#include <piejam/audio/dsp/biquad_filter.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/runtime/components/in_out_stream.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/internal_fx_component_factory.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameter_processor_factory.h>

namespace piejam::fx_modules::filter
{

namespace
{

using coeffs_t = audio::dsp::biquad<float>::coefficients;

auto
make_coefficent_converter_processor(audio::sample_rate const sample_rate)
{
    using namespace std::string_view_literals;
    static constexpr std::array s_input_names{"type"sv, "cutoff"sv, "res"sv};
    static constexpr std::array s_output_names{"coeffs1"sv, "coeffs2"sv};
    return audio::engine::make_event_converter_processor(
        [inv_sr = 1.f / sample_rate.as<float>()](
            int const type,
            float const cutoff,
            float const res) {
            using namespace audio::dsp::biquad_filter;

            switch (static_cast<filter::type>(type))
            {
                case type::lp2:
                    return std::tuple{
                        make_lp_coefficients(cutoff, res, inv_sr),
                        coeffs_t{}};

                case type::lp4:
                {
                    auto coeffs = make_lp_coefficients(cutoff, res, inv_sr);
                    return std::tuple{coeffs, coeffs};
                }

                case type::bp2:
                    return std::tuple{
                        make_bp_coefficients(cutoff, res, inv_sr),
                        coeffs_t{}};

                case type::bp4:
                {
                    constexpr float bw_min = 0.1f;
                    constexpr float bw_max = 6.f;
                    float const bw_oct = std::lerp(bw_max, bw_min, res);
                    float const half_bw = bw_oct * 0.5f;

                    float const fc_low =
                        std::max(cutoff * std::pow(2.f, -half_bw), 10.f);
                    float const fc_high =
                        std::min(cutoff * std::pow(2.f, +half_bw), 20000.f);
                    BOOST_ASSERT(fc_low < fc_high);

                    float const res_bp = std::lerp(0.f, 0.03152f, res);

                    return std::tuple{
                        make_hp_coefficients(fc_low, res_bp, inv_sr),
                        make_lp_coefficients(fc_high, res_bp, inv_sr),
                    };
                }

                case type::hp2:
                    return std::tuple{
                        make_hp_coefficients(cutoff, res, inv_sr),
                        coeffs_t{}};

                case type::hp4:
                {
                    auto coeffs = make_hp_coefficients(cutoff, res, inv_sr);
                    return std::tuple{coeffs, coeffs};
                }

                case type::br:
                    return std::tuple{
                        make_br_coefficients(cutoff, res, inv_sr),
                        coeffs_t{}};

                default:
                    return std::tuple{coeffs_t{}, coeffs_t{}};
            }
        },
        s_input_names,
        s_output_names,
        "filter_coeffs");
}

template <std::size_t... Channel>
class component final : public audio::engine::component
{
    static constexpr std::size_t num_channels = sizeof...(Channel);

public:
    component(runtime::internal_fx_component_factory_args const& args)
        : m_type_input_proc(args.param_procs.find_or_make_processor(
              args.fx_mod.parameters->at(parameter_key::type),
              "type"))
        , m_cutoff_input_proc(args.param_procs.find_or_make_processor(
              args.fx_mod.parameters->at(parameter_key::cutoff),
              "cutoff"))
        , m_resonance_input_proc(args.param_procs.find_or_make_processor(
              args.fx_mod.parameters->at(parameter_key::resonance),
              "res"))
        , m_coeffs_proc(make_coefficent_converter_processor(args.sample_rate))
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_filter1->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_filter2->outputs();
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
        using namespace audio::engine::endpoint_ports;

        m_filter1->connect(g);
        m_filter2->connect(g);

        audio::engine::connect_event(
            g,
            *m_type_input_proc,
            from<0>,
            *m_coeffs_proc,
            to<0>);

        audio::engine::connect_event(
            g,
            *m_cutoff_input_proc,
            from<0>,
            *m_coeffs_proc,
            to<1>);

        audio::engine::connect_event(
            g,
            *m_resonance_input_proc,
            from<0>,
            *m_coeffs_proc,
            to<2>);

        audio::engine::connect_event(
            g,
            *m_coeffs_proc,
            from<0>,
            *m_filter1,
            to<0>);

        audio::engine::connect_event(
            g,
            *m_coeffs_proc,
            from<1>,
            *m_filter2,
            to<0>);

        audio::engine::connect(g, *m_filter1, *m_filter2);
    }

private:
    std::shared_ptr<audio::engine::processor> m_type_input_proc;
    std::shared_ptr<audio::engine::processor> m_cutoff_input_proc;
    std::shared_ptr<audio::engine::processor> m_resonance_input_proc;
    std::unique_ptr<audio::engine::processor> m_coeffs_proc;
    std::unique_ptr<audio::engine::component> m_filter1{
        audio::components::make_biquad_filter(num_channels, "filter1")};
    std::unique_ptr<audio::engine::component> m_filter2{
        audio::components::make_biquad_filter(num_channels, "filter2")};
    std::array<audio::engine::graph_endpoint, 3> m_event_inputs{
        audio::engine::graph_endpoint{.proc = *m_type_input_proc, .port = 0},
        audio::engine::graph_endpoint{.proc = *m_cutoff_input_proc, .port = 0},
        audio::engine::graph_endpoint{
            .proc = *m_resonance_input_proc,
            .port = 0}};
};

template <std::size_t... Channel>
auto
make_component(
    runtime::internal_fx_component_factory_args const& args,
    std::index_sequence<Channel...>)
    -> std::unique_ptr<audio::engine::component>
{
    return runtime::components::wrap_with_in_out_stream(
        std::make_unique<component<Channel...>>(args),
        "filter_in_out",
        args.fx_mod.streams->at(
            std::to_underlying(fx_modules::filter::stream_key::in_out)),
        args.stream_procs,
        args.sample_rate.samples_for_duration(std::chrono::milliseconds(120)));
}

} // namespace

auto
make_component(runtime::internal_fx_component_factory_args const& args)
    -> std::unique_ptr<audio::engine::component>
{
    switch (args.fx_mod.bus_type)
    {
        case audio::bus_type::mono:
            return make_component(args, std::make_index_sequence<1>{});

        case audio::bus_type::stereo:
            return make_component(args, std::make_index_sequence<2>{});
    }
}

} // namespace piejam::fx_modules::filter
