// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/midi_assignment_processor.h>

#include <piejam/algorithm/index_of.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/midi/event.h>

#include <fmt/format.h>

#include <vector>

namespace piejam::runtime::processors
{

namespace
{

using external_midi_event_buffer =
        audio::engine::event_buffer<midi::external_event>;

class midi_assignment_processor final : public audio::engine::named_processor
{
public:
    midi_assignment_processor(midi_assignments_map const& midi_assigns)
        : m_midi_assignments(make_midi_assignments_vector(midi_assigns))
    {
        for (auto const& [id, ass] : midi_assigns)
        {
            switch (ass.control_type)
            {
                case midi_assignment::type::cc:
                    m_output_ports.emplace_back(
                            std::in_place_type<midi::cc_event>,
                            fmt::format(
                                    "{} CC {}",
                                    ass.channel,
                                    ass.control_id));
                    break;

                case midi_assignment::type::pc:
                    break;
            }
        }
    }

    auto type_name() const -> std::string_view override
    {
        return "midi_assignment";
    }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override
    {
        static std::array<audio::engine::event_port, 1> s_ports{
                audio::engine::event_port(
                        std::in_place_type<midi::external_event>,
                        "ext_midi")};

        return s_ports;
    }

    auto event_outputs() const -> event_ports override
    {
        return m_output_ports;
    }

    void process(audio::engine::process_context const& ctx) override
    {
        audio::engine::verify_process_context(*this, ctx);

        for (auto const& ev : ctx.event_inputs.get<midi::external_event>(0))
        {
            std::visit(
                    [this, &ctx, offset = ev.offset()](auto const& midi_ev) {
                        process_event(ctx, offset, midi_ev);
                    },
                    ev.value().event);
        }
    }

    void process_event(
            audio::engine::process_context const& ctx,
            std::size_t const offset,
            midi::channel_cc_event const& ev)
    {
        auto const out_index = algorithm::index_of(
                m_midi_assignments,
                midi_assignment{
                        .channel = ev.channel,
                        .control_type = midi_assignment::type::cc,
                        .control_id = ev.data.cc});

        if (out_index != npos)
        {
            auto& out = ctx.event_outputs.get<midi::cc_event>(out_index);
            out.insert(offset, ev.data);
        }
    }

private:
    static auto
    make_midi_assignments_vector(midi_assignments_map const& assignments)
            -> std::vector<midi_assignment>
    {
        std::vector<midi_assignment> result;
        result.reserve(assignments.size());

        std::ranges::copy(
                std::views::values(assignments),
                std::back_inserter(result));

        return result;
    }

    std::vector<midi_assignment> m_midi_assignments;
    std::vector<audio::engine::event_port> m_output_ports;
};

} // namespace

auto
make_midi_assignment_processor(midi_assignments_map const& midi_assigns)
        -> std::unique_ptr<audio::engine::processor>
{
    return std::make_unique<midi_assignment_processor>(midi_assigns);
}

} // namespace piejam::runtime::processors
