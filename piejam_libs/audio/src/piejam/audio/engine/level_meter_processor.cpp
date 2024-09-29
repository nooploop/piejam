// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/level_meter_processor.h>

#include <piejam/audio/dsp/mipp_iterator.h>
#include <piejam/audio/dsp/peak_level_meter.h>
#include <piejam/audio/dsp/rms_level_meter.h>
#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/sample_rate.h>

#include <boost/hof/match.hpp>

#include <algorithm>
#include <span>

namespace piejam::audio::engine
{

namespace
{

class peak_level_meter_processor final : public named_processor
{
public:
    peak_level_meter_processor(
            sample_rate sample_rate,
            std::string_view name = {})
        : engine::named_processor(name)
        , m_lm{sample_rate}
    {
    }

    [[nodiscard]]
    auto type_name() const noexcept -> std::string_view override
    {
        return "peak_level_meter";
    }

    [[nodiscard]]
    auto num_inputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    [[nodiscard]]
    auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    [[nodiscard]]
    auto event_inputs() const noexcept -> event_ports override
    {
        return {};
    }

    [[nodiscard]]
    auto event_outputs() const noexcept -> event_ports override
    {
        static std::array s_ports{
                event_port{std::in_place_type<float>, "level"},
        };
        return s_ports;
    }

    void process(engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        audio_slice::visit(
                boost::hof::match(
                        [this, bs = ctx.buffer_size](float const c) {
                            std::fill_n(std::back_inserter(m_lm), bs, c);
                        },
                        [this](audio_slice::span_t const buffer) {
                            std::ranges::copy(buffer, std::back_inserter(m_lm));
                        }),
                ctx.inputs[0].get());

        ctx.event_outputs.get<float>(0).insert(0, m_lm.level());
    }

private:
    dsp::peak_level_meter<float> m_lm;
};

class rms_level_meter_processor final : public named_processor
{
public:
    rms_level_meter_processor(
            sample_rate const sample_rate,
            std::string_view name = {})
        : engine::named_processor(name)
        , m_lm{sample_rate}
    {
    }

    [[nodiscard]]
    auto type_name() const noexcept -> std::string_view override
    {
        return "rms_level_meter";
    }

    [[nodiscard]]
    auto num_inputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    [[nodiscard]]
    auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    [[nodiscard]]
    auto event_inputs() const noexcept -> event_ports override
    {
        return {};
    }

    [[nodiscard]]
    auto event_outputs() const noexcept -> event_ports override
    {
        static std::array s_ports{
                event_port{std::in_place_type<float>, "level"},
        };
        return s_ports;
    }

    void process(engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        constexpr auto N = mipp::N<float>();
        BOOST_ASSERT(ctx.buffer_size % N == 0);
        audio_slice::visit(
                boost::hof::match(
                        [this, bs = ctx.buffer_size](float const c) {
                            std::fill_n(
                                    std::back_inserter(m_lm),
                                    bs / N,
                                    mipp::Reg<float>(c));
                        },
                        [this](audio_slice::span_t const buffer) {
                            std::ranges::copy(
                                    dsp::mipp_range(buffer),
                                    std::back_inserter(m_lm));
                        }),
                ctx.inputs[0].get());

        ctx.event_outputs.get<float>(0).insert(0, m_lm.level());
    }

private:
    dsp::rms_level_meter<> m_lm;
};

} // namespace

auto
make_peak_level_meter_processor(sample_rate sr, std::string_view name)
        -> std::unique_ptr<processor>
{
    return std::make_unique<peak_level_meter_processor>(sr, name);
}

auto
make_rms_level_meter_processor(sample_rate sr, std::string_view name)
        -> std::unique_ptr<processor>
{
    return std::make_unique<rms_level_meter_processor>(sr, name);
}

} // namespace piejam::audio::engine
