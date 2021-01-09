// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/select_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/single_event_input_processor.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>

#include <boost/assert.hpp>

#include <array>

namespace piejam::audio::engine
{

namespace
{

class select_processor final
    : public named_processor
    , public single_event_input_processor<select_processor, std::size_t>
{
public:
    select_processor(std::size_t const num_inputs, std::string_view const& name)
        : named_processor(name)
        , m_num_inputs(num_inputs)
    {
    }

    auto type_name() const -> std::string_view override { return "select"; }

    auto num_inputs() const -> std::size_t override { return m_num_inputs; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto event_inputs() const -> event_ports override
    {
        static std::array s_ports{
                event_port(std::in_place_type<std::size_t>, "select")};
        return s_ports;
    }

    auto event_outputs() const -> event_ports override { return {}; }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        process_sliced_on_events(ctx);
    }

    void process_without_events(process_context const& ctx)
    {
        ctx.results[0] = m_selected < m_num_inputs ? ctx.inputs[m_selected]
                                                   : audio_slice{};
    }

    void process_with_starting_event(
            process_context const& ctx,
            std::size_t const index)
    {
        m_selected = index;

        ctx.results[0] = m_selected < m_num_inputs ? ctx.inputs[m_selected]
                                                   : audio_slice{};
    }

    void process_event_slice(
            process_context const& ctx,
            std::size_t const from_offset,
            std::size_t const to_offset,
            std::size_t const next_index)
    {
        auto const subslice_size = to_offset - from_offset;
        copy(m_selected < m_num_inputs ? subslice(
                                                 ctx.inputs[m_selected].get(),
                                                 from_offset,
                                                 subslice_size)
                                       : audio_slice(),
             std::span<float>(
                     ctx.outputs[0].data() + from_offset,
                     subslice_size));
        m_selected = next_index;
    }

    void process_final_slice(
            process_context const& ctx,
            std::size_t const from_offset)
    {
        auto const subslice_size = ctx.buffer_size - from_offset;
        copy(m_selected < m_num_inputs ? subslice(
                                                 ctx.inputs[m_selected].get(),
                                                 from_offset,
                                                 subslice_size)
                                       : audio_slice(),
             std::span<float>(
                     ctx.outputs[0].data() + from_offset,
                     subslice_size));

        ctx.results[0] = ctx.outputs[0];
    }

private:
    std::size_t m_selected{};
    std::size_t m_num_inputs{};
};

} // namespace

auto
make_select_processor(std::size_t num_inputs, std::string_view const& name)
        -> std::unique_ptr<processor>
{
    BOOST_ASSERT(num_inputs > 0);
    return std::make_unique<select_processor>(num_inputs, name);
}

} // namespace piejam::audio::engine
