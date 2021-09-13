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
            std::size_t const smooth_length,
            std::string_view const& name)
        : named_processor(name)
        , m_smooth_length(smooth_length)
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "e_to_a";
    }

    auto num_inputs() const noexcept -> std::size_t override { return 0; }
    auto num_outputs() const noexcept -> std::size_t override { return 1; }

    auto event_inputs() const noexcept -> event_ports override
    {
        static std::array s_ports{event_port(std::in_place_type<float>, "ev")};
        return s_ports;
    }
    auto event_outputs() const noexcept -> event_ports override { return {}; }

    void process(engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        ctx.results[0] = ctx.outputs[0];

        process_sliced(ctx);
    }

    void process_buffer(process_context const& ctx)
    {
        if (m_smoother.is_running())
        {
            std::copy_n(
                    m_smoother.input_iterator(),
                    ctx.buffer_size,
                    ctx.outputs[0].begin());
        }
        else
        {
            ctx.results[0] = m_smoother.current();
        }
    }

    void process_slice(
            process_context const& ctx,
            std::size_t const from_offset,
            std::size_t const to_offset)
    {
        auto const n = to_offset - from_offset;
        auto it_out = std::next(ctx.outputs[0].begin(), from_offset);

        if (m_smoother.is_running())
        {
            std::copy_n(m_smoother.input_iterator(), n, std::move(it_out));
        }
        else
        {
            std::fill_n(std::move(it_out), n, m_smoother.current());
        }
    }

    void process_event_value(
            process_context const&,
            std::size_t /*offset*/,
            float const value)
    {
        m_smoother.set(value, m_smooth_length);
    }

private:
    std::size_t const m_smooth_length{};
    smoother<float> m_smoother;
};

} // namespace

auto
make_event_to_audio_processor(
        std::size_t const smooth_length,
        std::string_view const& name) -> std::unique_ptr<processor>
{
    return std::make_unique<event_to_audio_processor>(smooth_length, name);
}

} // namespace piejam::audio::engine
