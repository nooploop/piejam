// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/event_to_audio_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/single_event_input_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/smoother.h>

#include <type_traits>

namespace piejam::audio::engine
{

namespace
{

class event_to_audio_processor final
    : public named_processor
    , public single_event_input_processor<event_to_audio_processor, float>
{
public:
    event_to_audio_processor(
            std::string_view const& name,
            std::size_t smooth_length)
        : named_processor(name)
        , m_smooth_length(smooth_length)
    {
    }

    auto type_name() const -> std::string_view override { return "e_to_a"; }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto event_inputs() const -> event_ports override
    {
        static std::array s_ports{event_port(std::in_place_type<float>, "ev")};
        return s_ports;
    }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        process_sliced_on_events(ctx);
    }

    void process_without_events(process_context const& ctx)
    {
        if (m_smoother.is_running())
        {
            std::copy_n(
                    m_smoother.advance_iterator(),
                    ctx.buffer_size,
                    ctx.outputs[0].begin());

            ctx.results[0] = ctx.outputs[0];
        }
        else
        {
            ctx.results[0] = m_smoother.current();
        }
    }

    void
    process_with_starting_event(process_context const& ctx, float const value)
    {
        m_smoother.set(value, m_smooth_length);

        std::copy_n(
                m_smoother.advance_iterator(),
                ctx.buffer_size,
                ctx.outputs[0].begin());

        ctx.results[0] = ctx.outputs[0];
    }

    void process_event_slice(
            process_context const& ctx,
            std::size_t const from_offset,
            std::size_t const to_offset,
            float const next_value)
    {
        std::copy_n(
                m_smoother.advance_iterator(),
                to_offset - from_offset,
                std::next(ctx.outputs[0].begin(), from_offset));
        m_smoother.set(next_value, m_smooth_length);
    }

    void process_final_slice(
            process_context const& ctx,
            std::size_t const from_offset)
    {
        std::copy_n(
                m_smoother.advance_iterator(),
                ctx.buffer_size - from_offset,
                std::next(ctx.outputs[0].begin(), from_offset));

        ctx.results[0] = ctx.outputs[0];
    }

private:
    std::size_t const m_smooth_length{};
    smoother<float> m_smoother;
};

} // namespace

auto
make_event_to_audio_processor(
        std::string_view const& name,
        std::size_t const smooth_length) -> std::unique_ptr<processor>
{
    return std::make_unique<event_to_audio_processor>(name, smooth_length);
}

} // namespace piejam::audio::engine
