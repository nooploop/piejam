// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/filter/filter_component.h>

#include <piejam/audio/dsp/biquad.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/single_event_input_processor.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/stream_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/modules/filter/filter_module.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/runtime/processors/stream_processor_factory.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>
#include <boost/hof/match.hpp>

#include <cmath>
#include <numbers>

namespace piejam::runtime::modules::filter
{

namespace
{

struct coefficients
{
    using coeffs_t = audio::dsp::biquad<float>::coefficients;

    type tp{type::bypass};
    coeffs_t coeffs;
};

static constexpr auto
calc_b2(float const phi, float const Q) noexcept -> float
{
    float const two_Q = 2.f * Q;
    float const sin_phi = std::sin(phi);
    return (two_Q - sin_phi) / (two_Q + sin_phi);
}

static constexpr auto
calc_b1(float const phi, float const b2) -> float
{
    return -(1 + b2) * std::cos(phi);
}

static constexpr auto
calc_phi(float const freq_c, float const inv_sr) noexcept -> float
{
    return 2.f * std::numbers::pi * freq_c * inv_sr;
}

static constexpr auto
calc_Q(float const res) noexcept -> float
{
    constexpr float const sqrt_half = std::sqrt(.5f);
    return res * (10.f - sqrt_half) + sqrt_half;
}

using lp2_tag = std::integral_constant<type, type::lp2>;
using hp2_tag = std::integral_constant<type, type::hp2>;
using lp4_tag = std::integral_constant<type, type::lp4>;
using hp4_tag = std::integral_constant<type, type::hp4>;

constexpr auto
make_coefficients(
        lp2_tag tag,
        float const cutoff,
        float const res,
        float const inv_sr) noexcept
{
    float const phi = calc_phi(cutoff, inv_sr);
    float const Q = calc_Q(res);
    float const b2 = calc_b2(phi, Q);
    float const b1 = calc_b1(phi, b2);
    float const a0 = .25f * (1.f + b1 + b2);
    return coefficients{
            .tp = tag,
            .coeffs = {.b2 = b2, .b1 = b1, .a0 = a0, .a1 = 2.f * a0, .a2 = a0}};
}

constexpr auto
make_coefficients(
        hp2_tag tag,
        float const cutoff,
        float const res,
        float const inv_sr) noexcept
{
    float const phi = calc_phi(cutoff, inv_sr);
    float const Q = calc_Q(res);
    float const b2 = calc_b2(phi, Q);
    float const b1 = calc_b1(phi, b2);
    float const a0 = .25f * (1.f - b1 + b2);
    return coefficients{
            .tp = tag,
            .coeffs =
                    {.b2 = b2, .b1 = b1, .a0 = a0, .a1 = -2.f * a0, .a2 = a0}};
}

constexpr auto
make_coefficients(
        lp4_tag tag,
        float const cutoff,
        float const res,
        float const inv_sr) noexcept
{
    // same coefficients as for lp2, just replace the type
    auto coeffs = make_coefficients(lp2_tag{}, cutoff, res, inv_sr);
    coeffs.tp = tag;
    return coeffs;
}

constexpr auto
make_coefficients(
        hp4_tag tag,
        float const cutoff,
        float const res,
        float const inv_sr) noexcept
{
    // same coefficients as for hp2, just replace the type
    auto coeffs = make_coefficients(hp2_tag{}, cutoff, res, inv_sr);
    coeffs.tp = tag;
    return coeffs;
}

auto
make_coefficent_converter_processor(audio::sample_rate const& sample_rate)
{
    static constexpr std::string_view s_type_name = "type";
    static constexpr std::string_view s_cutoff_name = "cutoff";
    static constexpr std::string_view s_res_name = "res";
    static constexpr std::array s_converter_input_names{
            s_type_name,
            s_cutoff_name,
            s_res_name};
    return audio::engine::make_event_converter_processor(
            [inv_sr = 1.f / sample_rate.as_float()](
                    int const type,
                    float const cutoff,
                    float const res) {
                switch (type)
                {
                    case to_underlying(type::lp2):
                        return make_coefficients(
                                lp2_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::hp2):
                        return make_coefficients(
                                hp2_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::lp4):
                        return make_coefficients(
                                lp4_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::hp4):
                        return make_coefficients(
                                hp4_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    default:
                        return coefficients{};
                }
            },
            std::span(s_converter_input_names),
            "coeffs",
            "make_coeff");
}

class processor final
    : public audio::engine::named_processor
    , public audio::engine::
              single_event_input_processor<processor, coefficients>
{
public:
    processor(std::string_view const& name)
        : named_processor(name)
    {
    }

    auto type_name() const -> std::string_view override { return "filter"; }

    auto num_inputs() const -> std::size_t override { return 1; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto event_inputs() const -> event_ports override
    {
        static std::array s_ports{audio::engine::event_port{
                std::in_place_type<coefficients>,
                "coeffs"}};
        return s_ports;
    }

    auto event_outputs() const -> event_ports override { return {}; }

    void process(audio::engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        ctx.results[0] = ctx.outputs[0];

        process_sliced(ctx);
    }

    void process_buffer(audio::engine::process_context const& ctx)
    {
        if (m_type == type::bypass)
            ctx.results[0] = ctx.inputs[0];
        else
            process_slice(ctx, 0, ctx.buffer_size);
    }

    void process_slice(
            audio::engine::process_context const& ctx,
            std::size_t const from_offset,
            std::size_t const to_offset)
    {
        std::visit(
                boost::hof::match(
                        [this, &ctx, from_offset, to_offset](float const c) {
                            std::ranges::generate_n(
                                    std::next(
                                            ctx.outputs[0].begin(),
                                            from_offset),
                                    to_offset - from_offset,
                                    [this, c]() {
                                        return m_biquad_second.process(
                                                m_biquad_first.process(c));
                                    });
                        },
                        [this, &ctx, from_offset, to_offset](
                                audio::engine::audio_slice::span_t const& buf) {
                            std::ranges::transform(
                                    std::next(buf.begin(), from_offset),
                                    std::next(buf.begin(), to_offset),
                                    std::next(
                                            ctx.outputs[0].begin(),
                                            from_offset),
                                    [this](float const x0) {
                                        return m_biquad_second.process(
                                                m_biquad_first.process(x0));
                                    });
                        }),
                ctx.inputs[0].get().as_variant());
    }

    void process_event_value(
            audio::engine::process_context const& /*ctx*/,
            std::size_t const /*offset*/,
            coefficients const& value)
    {
        m_type = value.tp;

        m_biquad_first.coeffs = value.coeffs;

        if (m_type == type::lp4 || m_type == type::hp4)
            m_biquad_second.coeffs = value.coeffs;
        else
            m_biquad_second.coeffs = {};
    }

private:
    type m_type{type::bypass};
    audio::dsp::biquad<float> m_biquad_first;
    audio::dsp::biquad<float> m_biquad_second;
};

class component final : public audio::engine::component
{

public:
    component(
            fx::module const& fx_mod,
            audio::sample_rate const& sample_rate,
            parameter_processor_factory& proc_factory,
            processors::stream_processor_factory& stream_proc_factory,
            std::string_view const& /*name*/)
        : m_type_input_proc(processors::make_parameter_processor(
                  proc_factory,
                  fx_mod.parameters->at(to_underlying(parameter_key::type)),
                  "type"))
        , m_cutoff_input_proc(processors::make_parameter_processor(
                  proc_factory,
                  fx_mod.parameters->at(to_underlying(parameter_key::cutoff)),
                  "cutoff"))
        , m_resonance_input_proc(processors::make_parameter_processor(
                  proc_factory,
                  fx_mod.parameters->at(
                          to_underlying(parameter_key::resonance)),
                  "res"))
        , m_event_converter_proc(
                  make_coefficent_converter_processor(sample_rate))
        , m_filter_left_proc(std::make_unique<processor>("left"))
        , m_filter_right_proc(std::make_unique<processor>("right"))
        , m_in_out_stream_proc(stream_proc_factory.make_processor(
                  fx_mod.streams->at(
                          to_underlying(modules::filter::stream_key::in_out)),
                  4,
                  sample_rate.to_samples(std::chrono::milliseconds(17 * 3)),
                  "filter_in_out"))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) const override
    {
        g.add_event_wire({*m_type_input_proc, 0}, {*m_event_converter_proc, 0});
        g.add_event_wire(
                {*m_cutoff_input_proc, 0},
                {*m_event_converter_proc, 1});
        g.add_event_wire(
                {*m_resonance_input_proc, 0},
                {*m_event_converter_proc, 2});

        g.add_event_wire(
                {*m_event_converter_proc, 0},
                {*m_filter_left_proc, 0});
        g.add_event_wire(
                {*m_event_converter_proc, 0},
                {*m_filter_right_proc, 0});

        g.add_wire({*m_input_left_proc, 0}, {*m_filter_left_proc, 0});
        g.add_wire({*m_input_right_proc, 0}, {*m_filter_right_proc, 0});

        g.add_wire({*m_input_left_proc, 0}, {*m_in_out_stream_proc, 0});
        g.add_wire({*m_input_right_proc, 0}, {*m_in_out_stream_proc, 1});

        g.add_wire({*m_filter_left_proc, 0}, {*m_in_out_stream_proc, 2});
        g.add_wire({*m_filter_right_proc, 0}, {*m_in_out_stream_proc, 3});
    }

private:
    std::shared_ptr<audio::engine::processor> m_type_input_proc;
    std::shared_ptr<audio::engine::processor> m_cutoff_input_proc;
    std::shared_ptr<audio::engine::processor> m_resonance_input_proc;
    std::unique_ptr<audio::engine::processor> m_event_converter_proc;
    std::unique_ptr<audio::engine::processor> m_input_left_proc{
            audio::engine::make_identity_processor()};
    std::unique_ptr<audio::engine::processor> m_input_right_proc{
            audio::engine::make_identity_processor()};
    std::unique_ptr<processor> m_filter_left_proc;
    std::unique_ptr<processor> m_filter_right_proc;
    std::shared_ptr<audio::engine::processor> m_in_out_stream_proc;
    std::array<audio::engine::graph_endpoint, 2> m_inputs{
            audio::engine::graph_endpoint{
                    .proc = *m_input_left_proc,
                    .port = 0},
            audio::engine::graph_endpoint{
                    .proc = *m_input_right_proc,
                    .port = 0}};
    std::array<audio::engine::graph_endpoint, 2> m_outputs{
            audio::engine::graph_endpoint{
                    .proc = *m_filter_left_proc,
                    .port = 0},
            audio::engine::graph_endpoint{
                    .proc = *m_filter_right_proc,
                    .port = 0}};
    std::array<audio::engine::graph_endpoint, 3> m_event_inputs{
            audio::engine::graph_endpoint{
                    .proc = *m_type_input_proc,
                    .port = 0},
            audio::engine::graph_endpoint{
                    .proc = *m_cutoff_input_proc,
                    .port = 0},
            audio::engine::graph_endpoint{
                    .proc = *m_resonance_input_proc,
                    .port = 0}};
};

} // namespace

auto
make_component(
        fx::module const& fx_mod,
        audio::sample_rate const& sample_rate,
        parameter_processor_factory& proc_factory,
        processors::stream_processor_factory& stream_proc_factory,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<component>(
            fx_mod,
            sample_rate,
            proc_factory,
            stream_proc_factory,
            name);
}

} // namespace piejam::runtime::modules::filter
