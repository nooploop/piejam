// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/midi_input_processor.h>

#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/midi/event.h>
#include <piejam/midi/event_handler.h>
#include <piejam/midi/input_event_handler.h>

namespace piejam::runtime::processors
{

namespace
{

using external_midi_event_buffer =
        audio::engine::event_buffer<midi::external_event>;

class midi_input_processor final : public audio::engine::named_processor
{
public:
    struct event_handler final : midi::event_handler
    {
        event_handler(external_midi_event_buffer& event_out_buffer)
            : m_event_out_buffer(event_out_buffer)
        {
        }

        void process(midi::external_event const& ev) override
        {
            m_event_out_buffer.insert(0, ev);
        }

    private:
        external_midi_event_buffer& m_event_out_buffer;
    };

    midi_input_processor(std::unique_ptr<midi::input_event_handler> midi_in)
        : m_midi_in(std::move(midi_in))
    {
        BOOST_ASSERT(m_midi_in);
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "midi_input";
    }

    auto num_inputs() const noexcept -> std::size_t override { return 0; }
    auto num_outputs() const noexcept -> std::size_t override { return 0; }

    auto event_inputs() const noexcept -> event_ports override { return {}; }
    auto event_outputs() const noexcept -> event_ports override
    {
        static std::array const s_event_outs{audio::engine::event_port(
                std::in_place_type<midi::external_event>,
                "ext_midi")};

        return s_event_outs;
    }

    void process(audio::engine::process_context const& ctx) override
    {
        audio::engine::verify_process_context(*this, ctx);

        event_handler ev_handler(
                ctx.event_outputs.get<midi::external_event>(0));
        m_midi_in->process(ev_handler);
    }

private:
    std::unique_ptr<midi::input_event_handler> m_midi_in;
};

} // namespace

auto
make_midi_input_processor(std::unique_ptr<midi::input_event_handler> midi_in)
        -> std::unique_ptr<audio::engine::processor>
{
    return std::make_unique<midi_input_processor>(std::move(midi_in));
}

} // namespace piejam::runtime::processors
