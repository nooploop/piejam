// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/filter/filter_component.h>

#include <piejam/audio/dsp/biquad_filter.h>
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
#include <piejam/integral_constant.h>
#include <piejam/math.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/modules/filter/filter_module.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/runtime/processors/stream_processor_factory.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>
#include <boost/hof/match.hpp>
#include <boost/mp11/map.hpp>

#include <cmath>
#include <numbers>

namespace piejam::runtime::modules::filter
{

namespace
{

struct event_value
{
    using coeffs_t = audio::dsp::biquad<float>::coefficients;

    type tp{type::bypass};
    coeffs_t coeffs;
};

using lp2_tag = make_integral_constant<type::lp2>;
using lp4_tag = make_integral_constant<type::lp4>;
using bp2_tag = make_integral_constant<type::bp2>;
using bp4_tag = make_integral_constant<type::bp4>;
using hp2_tag = make_integral_constant<type::hp2>;
using hp4_tag = make_integral_constant<type::hp4>;
using br_tag = make_integral_constant<type::br>;

namespace biqflt = audio::dsp::biquad_filter;

using tag_make_coefficients_map = boost::mp11::mp_list<
        std::pair<
                lp2_tag,
                make_integral_constant<&biqflt::make_lp_coefficients<float>>>,
        std::pair<
                lp4_tag,
                make_integral_constant<&biqflt::make_lp_coefficients<float>>>,
        std::pair<
                bp2_tag,
                make_integral_constant<&biqflt::make_bp_coefficients<float>>>,
        std::pair<
                bp4_tag,
                make_integral_constant<&biqflt::make_bp_coefficients<float>>>,
        std::pair<
                hp2_tag,
                make_integral_constant<&biqflt::make_hp_coefficients<float>>>,
        std::pair<
                hp4_tag,
                make_integral_constant<&biqflt::make_hp_coefficients<float>>>,
        std::pair<
                br_tag,
                make_integral_constant<&biqflt::make_br_coefficients<float>>>>;

template <class Tag>
constexpr auto
make_coefficients(
        Tag tag,
        float const cutoff,
        float const res,
        float const inv_sr) noexcept
{
    using make_t = typename boost::mp11::
            mp_map_find<tag_make_coefficients_map, Tag>::second_type;
    return event_value{
            .tp = tag,
            .coeffs = make_t::value(cutoff, res, inv_sr)};
}

auto
make_coefficent_converter_processor(audio::sample_rate const sample_rate)
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

                    case to_underlying(type::lp4):
                        return make_coefficients(
                                lp4_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::bp2):
                        return make_coefficients(
                                bp2_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::bp4):
                        return make_coefficients(
                                bp4_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::hp2):
                        return make_coefficients(
                                hp2_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::hp4):
                        return make_coefficients(
                                hp4_tag{},
                                cutoff,
                                res,
                                inv_sr);

                    case to_underlying(type::br):
                        return make_coefficients(br_tag{}, cutoff, res, inv_sr);

                    default:
                        return event_value{};
                }
            },
            std::span(s_converter_input_names),
            "coeffs",
            "make_coeff");
}

class processor final
    : public audio::engine::named_processor
    , public audio::engine::
              single_event_input_processor<processor, event_value>
{
public:
    processor(std::string_view const name)
        : named_processor(name)
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "filter";
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return 1;
    }
    auto num_outputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    auto event_inputs() const noexcept -> event_ports override
    {
        static std::array s_ports{audio::engine::event_port{
                std::in_place_type<event_value>,
                "coeffs"}};
        return s_ports;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        return {};
    }

    void process(audio::engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        ctx.results[0] = ctx.outputs[0];

        process_sliced(ctx);
    }

    void process_buffer(audio::engine::process_context const& ctx)
    {
        if (m_type == type::bypass)
        {
            ctx.results[0] = ctx.inputs[0];
        }
        else
        {
            process_slice(ctx, 0, ctx.buffer_size);
        }
    }

    void process_slice(
            audio::engine::process_context const& ctx,
            std::size_t const offset,
            std::size_t const count)
    {
        using audio::engine::audio_slice;

        auto out_it = std::next(ctx.outputs[0].begin(), offset);

        audio_slice::visit(
                boost::hof::match(
                        [=, this, &out_it](float const c) {
                            std::ranges::generate_n(
                                    out_it,
                                    count,
                                    std::bind_front(m_process_sample, this, c));
                        },
                        [=, this, &out_it](audio_slice::span_t const& buf) {
                            std::ranges::transform(
                                    buf,
                                    out_it,
                                    std::bind_front(m_process_sample, this));
                        }),
                subslice(ctx.inputs[0].get(), offset, count));
    }

    void process_event(
            audio::engine::process_context const& /*ctx*/,
            audio::engine::event<event_value> const& ev)
    {
        m_type = ev.value().tp;

        m_biquad_first.coeffs = ev.value().coeffs;

        switch (m_type)
        {
            case type::lp4:
            case type::bp4:
            case type::hp4:
                m_biquad_second.coeffs = ev.value().coeffs;
                m_process_sample = &processor::process_sample;
                break;

            default:
                m_biquad_second.coeffs = {};
                m_process_sample = &processor::process_sample_first_only;
                break;
        }
    }

    auto process_sample(float const x0) -> float
    {
        return m_biquad_second.process(m_biquad_first.process(x0));
    }

    auto process_sample_first_only(float const x0) -> float
    {
        return m_biquad_first.process(x0);
    }

private:
    type m_type{type::bypass};
    audio::dsp::biquad<float> m_biquad_first;
    audio::dsp::biquad<float> m_biquad_second;
    using process_sample_t = float (processor::*)(float);
    process_sample_t m_process_sample{&processor::process_sample_first_only};
};

class component final : public audio::engine::component
{

public:
    component(
            fx::module const& fx_mod,
            audio::sample_rate const sample_rate,
            parameter_processor_factory& proc_factory,
            processors::stream_processor_factory& stream_proc_factory,
            std::string_view const /*name*/)
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
        g.event.insert({*m_type_input_proc, 0}, {*m_event_converter_proc, 0});
        g.event.insert({*m_cutoff_input_proc, 0}, {*m_event_converter_proc, 1});
        g.event.insert(
                {*m_resonance_input_proc, 0},
                {*m_event_converter_proc, 2});

        g.event.insert({*m_event_converter_proc, 0}, {*m_filter_left_proc, 0});
        g.event.insert({*m_event_converter_proc, 0}, {*m_filter_right_proc, 0});

        g.audio.insert({*m_input_left_proc, 0}, {*m_filter_left_proc, 0});
        g.audio.insert({*m_input_right_proc, 0}, {*m_filter_right_proc, 0});

        g.audio.insert({*m_input_left_proc, 0}, {*m_in_out_stream_proc, 0});
        g.audio.insert({*m_input_right_proc, 0}, {*m_in_out_stream_proc, 1});

        g.audio.insert({*m_filter_left_proc, 0}, {*m_in_out_stream_proc, 2});
        g.audio.insert({*m_filter_right_proc, 0}, {*m_in_out_stream_proc, 3});
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
        audio::sample_rate const sample_rate,
        parameter_processor_factory& proc_factory,
        processors::stream_processor_factory& stream_proc_factory,
        std::string_view const name)
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
