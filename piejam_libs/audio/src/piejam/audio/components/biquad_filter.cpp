// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/biquad_filter.h>

#include <piejam/audio/dsp/biquad.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_identity_processor.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/single_event_input_processor.h>
#include <piejam/audio/slice.h>
#include <piejam/audio/slice_algorithms.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/range/indices.h>
#include <piejam/range/indirected.h>
#include <piejam/range/iota.h>

#include <boost/assert.hpp>

#include <format>
#include <vector>

namespace piejam::audio::components
{

namespace
{

using coeffs_t = audio::dsp::biquad<float>::coefficients;

class processor final
    : public audio::engine::named_processor
    , public audio::engine::single_event_input_processor<processor, coeffs_t>
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
        static std::array s_ports{
            audio::engine::event_port{std::in_place_type<coeffs_t>, "coeffs"}};
        return s_ports;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        return {};
    }

    void process(audio::engine::process_context const& ctx) override
    {
        ctx.results[0] = ctx.outputs[0];

        process_sliced(ctx);
    }

    void process_buffer(audio::engine::process_context const& ctx)
    {
        visit(
            [this, out = ctx.outputs[0]](auto const in) { process(in, out); },
            ctx.inputs[0].get());
    }

    void process_slice(
        audio::engine::process_context const& ctx,
        std::size_t const offset,
        std::size_t const count)
    {
        visit(
            [this, out = ctx.outputs[0].subspan(offset, count)](auto const in) {
                process(in, out);
            },
            subslice(ctx.inputs[0].get(), offset, count));
    }

    void process_event(
        audio::engine::process_context const& /*ctx*/,
        audio::engine::event<coeffs_t> const& ev)
    {
        m_biquad.coeffs = ev.value();
    }

private:
    void process(float const c, std::span<float> out)
    {
        std::ranges::generate(out, [this, c]() { return m_biquad.process(c); });
    }

    void process(audio::slice<float>::span_t const in, std::span<float> out)
    {
        std::ranges::transform(in, out.begin(), [this](float const s) {
            return m_biquad.process(s);
        });
    }

    audio::dsp::biquad<float> m_biquad;
};

[[nodiscard]]
auto
filter_channel_name(
    std::size_t const ch,
    std::size_t const num_channels,
    std::string_view const prefix)
{
    switch (num_channels)
    {
        case 2:
            switch (ch)
            {
                case 0:
                    return std::format("{} L", prefix);
                case 1:
                    return std::format("{} R", prefix);
            }
            break;

        case 1:
            return std::format("{} M", prefix);
    }

    return std::format("{} {}", prefix, ch + 1);
}

class component final : public engine::component
{
public:
    explicit component(
        std::size_t const num_channels,
        std::string_view const name)
        : m_procs{algorithm::transform_to_vector(
              range::iota(num_channels),
              [=](std::size_t const ch)
                  -> std::unique_ptr<audio::engine::processor> {
                  return std::make_unique<processor>(
                      filter_channel_name(ch, num_channels, name));
              })}
    {
    }

    [[nodiscard]]
    auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    [[nodiscard]]
    auto outputs() const -> endpoints override
    {
        return m_outputs;
    }

    [[nodiscard]]
    auto event_inputs() const -> endpoints override
    {
        return m_event_inputs;
    }

    [[nodiscard]]
    auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(engine::graph& g) const override
    {
        for (std::size_t ch : range::indices(m_inputs))
        {
            using namespace engine::endpoint_ports;
            engine::connect_event(
                g,
                *m_coeffs_event_input_proc,
                from<0>,
                *m_procs[ch],
                to<0>);
        }
    }

private:
    std::vector<std::unique_ptr<engine::processor>> m_procs;

    std::vector<engine::graph_endpoint> m_inputs{algorithm::transform_to_vector(
        m_procs | range::indirected,
        engine::make_graph_endpoint<0>)};
    std::vector<engine::graph_endpoint> m_outputs{m_inputs};

    std::unique_ptr<audio::engine::processor> m_coeffs_event_input_proc{
        engine::make_event_identity_processor<coeffs_t>()};
    std::array<engine::graph_endpoint, 1> m_event_inputs{
        engine::make_graph_endpoint<0>(*m_coeffs_event_input_proc)};
};

} // namespace

auto
make_biquad_filter(std::size_t num_channels, std::string_view name)
    -> std::unique_ptr<engine::component>
{
    BOOST_ASSERT(num_channels > 0);
    return std::make_unique<component>(num_channels, name);
}

} // namespace piejam::audio::components
