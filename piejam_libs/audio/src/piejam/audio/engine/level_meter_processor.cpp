// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/level_meter_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/level_meter.h>
#include <piejam/audio/sample_rate.h>

#include <boost/hof/match.hpp>

#include <algorithm>
#include <span>

namespace piejam::audio::engine
{

namespace
{

class level_meter_processor final : public named_processor
{
public:
    level_meter_processor(sample_rate, std::string_view name = {});

    [[nodiscard]] auto type_name() const noexcept -> std::string_view override
    {
        return "level_meter";
    }

    [[nodiscard]] auto num_inputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    [[nodiscard]] auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    [[nodiscard]] auto event_inputs() const noexcept -> event_ports override
    {
        return {};
    }

    [[nodiscard]] auto event_outputs() const noexcept -> event_ports override
    {
        static std::array s_ports{
                event_port{std::in_place_type<float>, "peak_level"},
                event_port{std::in_place_type<float>, "rms_level"},
        };
        return s_ports;
    }

    void process(engine::process_context const&) override;

private:
    level_meter m_lm;
};

level_meter_processor::level_meter_processor(
        sample_rate const sample_rate,
        std::string_view const name)
    : engine::named_processor(name)
    , m_lm{sample_rate}
{
}

void
level_meter_processor::process(engine::process_context const& ctx)
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

    ctx.event_outputs.get<float>(0).insert(0, m_lm.peak_level());
    ctx.event_outputs.get<float>(1).insert(0, m_lm.rms_level());
}

} // namespace

auto
make_level_meter_processor(sample_rate sr, std::string_view name)
        -> std::unique_ptr<processor>
{
    return std::make_unique<level_meter_processor>(sr, name);
}

} // namespace piejam::audio::engine
