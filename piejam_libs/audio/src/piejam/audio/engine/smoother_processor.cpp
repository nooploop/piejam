// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/smoother_processor.h>

#include <piejam/audio/dsp/smoother.h>
#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/single_event_input_processor.h>
#include <piejam/audio/engine/verify_process_context.h>

#include <boost/assert.hpp>

#include <type_traits>

namespace piejam::audio::engine
{

namespace
{

class event_to_audio_smoother_processor final
    : public named_processor
    , public single_event_input_processor<
              event_to_audio_smoother_processor,
              float>
{
public:
    event_to_audio_smoother_processor(
            std::size_t const smooth_length,
            std::string_view const name)
        : named_processor{name}
        , m_smooth_length{smooth_length}
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "e_to_a_smooth";
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    auto num_outputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    auto event_inputs() const noexcept -> event_ports override
    {
        static std::array s_ports{event_port(std::in_place_type<float>, "ev")};
        return s_ports;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        return {};
    }

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
                    m_smoother.generator(),
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
            std::size_t const offset,
            std::size_t const count)
    {
        auto it_out = std::next(ctx.outputs[0].begin(), offset);

        if (m_smoother.is_running())
        {
            std::copy_n(m_smoother.generator(), count, std::move(it_out));
        }
        else
        {
            std::fill_n(std::move(it_out), count, m_smoother.current());
        }
    }

    void process_event(process_context const&, event<float> const& ev)
    {
        m_smoother.set(ev.value(), m_smooth_length);
    }

private:
    std::size_t const m_smooth_length{};
    dsp::smoother<float> m_smoother;
};

class event_smoother_processor final
    : public named_processor
    , public single_event_input_processor<event_smoother_processor, float>
{
public:
    event_smoother_processor(
            std::size_t const smooth_length,
            std::size_t const smooth_steps,
            std::string_view const name)
        : named_processor{name}
        , m_smooth_steps{smooth_steps}
        , m_step_length{smooth_length / smooth_steps}
    {
        BOOST_ASSERT_MSG(
                smooth_length % smooth_steps == 0,
                "smooth_length should be dividable by smooth_steps");
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "smooth";
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    auto event_inputs() const noexcept -> event_ports override
    {
        static std::array s_ports{event_port(std::in_place_type<float>, "ev")};
        return s_ports;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        static std::array s_ports{
                event_port(std::in_place_type<float>, "sm_ev")};
        return s_ports;
    }

    void process(engine::process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        process_sliced(ctx);
    }

    void process_buffer(process_context const& ctx)
    {
        process_slice(ctx, 0, ctx.buffer_size);
    }

    void process_slice(
            process_context const& ctx,
            std::size_t const offset,
            std::size_t const count)
    {
        std::size_t samples_left = count;
        while (samples_left && m_smoother.is_running())
        {
            if (m_counter == 0u)
            {
                ctx.event_outputs.get<float>(0).insert(
                        offset + count - samples_left,
                        *(m_smoother.generator())++);

                if (m_smoother.is_running())
                {
                    m_counter = m_step_length;
                }
            }
            else
            {
                std::size_t const rem = std::min(samples_left, m_counter);

                samples_left -= rem;
                m_counter -= rem;
            }
        }
    }

    void process_event(process_context const&, event<float> const& ev)
    {
        m_smoother.set(ev.value(), m_smooth_steps);
        m_counter = m_step_length;
    }

private:
    std::size_t const m_smooth_steps;
    std::size_t const m_step_length;
    std::size_t m_counter{};
    dsp::smoother<float> m_smoother;
};

} // namespace

auto
make_event_to_audio_smoother_processor(
        std::size_t const smooth_length,
        std::string_view const name) -> std::unique_ptr<processor>
{
    return std::make_unique<event_to_audio_smoother_processor>(
            smooth_length,
            name);
}

auto
make_event_smoother_processor(
        std::size_t const smooth_length,
        std::size_t const smooth_steps,
        std::string_view const name) -> std::unique_ptr<processor>
{
    return std::make_unique<event_smoother_processor>(
            smooth_length,
            smooth_steps,
            name);
}

} // namespace piejam::audio::engine
