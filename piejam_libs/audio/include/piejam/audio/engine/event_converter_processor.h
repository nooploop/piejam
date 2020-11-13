// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/algorithm/string_join.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/lockstep_events.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/meta/decay_tuple_elements.h>

#include <fmt/format.h>

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include <tuple>

namespace piejam::audio::engine
{

template <class OutputGenerator>
class event_converter_processor final : public named_processor
{
public:
    using output_t = std::decay_t<
            boost::callable_traits::return_type_t<OutputGenerator>>;
    using inputs_t = meta::decay_tuple_elements_t<
            boost::callable_traits::args_t<OutputGenerator>>;

    static constexpr std::size_t num_event_inputs = std::tuple_size_v<inputs_t>;

    static_assert(num_event_inputs > 0);

    template <class F>
    event_converter_processor(
            F&& out_gen,
            std::span<std::string_view> const& input_names = {},
            std::string_view const& output_name = {},
            std::string_view const& name = {})
        : named_processor(name)
        , m_out_gen(std::forward<F>(out_gen))
        , m_input_event_ports(make_input_event_ports(
                  input_names,
                  make_input_index_sequence()))
        , m_output_event_ports(
                  {event_port(std::in_place_type<output_t>, output_name)})
    {
    }

    auto type_name() const -> std::string_view override { return "convert"; }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override
    {
        return m_input_event_ports;
    }

    auto event_outputs() const -> event_ports override
    {
        return m_output_event_ports;
    }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        process_events(ctx, make_input_index_sequence());
    }

private:
    template <std::size_t... I>
    void process_events(
            process_context const& ctx,
            std::index_sequence<I...> indices)
    {
        auto in_bufs = get_input_buffers(ctx, indices);
        auto& out_buf = ctx.event_outputs.get<output_t>(0);

        m_last = lockstep_events(
                [this, &out_buf](std::size_t offset, auto&&... args) {
                    out_buf.insert(
                            offset,
                            std::invoke(
                                    m_out_gen,
                                    std::forward<decltype(args)>(args)...));
                },
                m_last,
                std::get<I>(in_bufs)...);
    }

    static auto make_input_index_sequence()
    {
        return std::make_index_sequence<num_event_inputs>{};
    }

    template <std::size_t... I>
    static auto make_input_event_ports(
            std::span<std::string_view> const& names,
            std::index_sequence<I...>)
    {
        std::size_t const num_names = names.size();
        return std::array{event_port(
                std::in_place_type<std::tuple_element_t<I, inputs_t>>,
                I < num_names ? names[I] : std::string())...};
    }

    template <std::size_t... I>
    static auto
    get_input_buffers(process_context const& ctx, std::index_sequence<I...>)
    {
        return std::forward_as_tuple(
                ctx.event_inputs.get<std::tuple_element_t<I, inputs_t>>(I)...);
    }

    OutputGenerator m_out_gen;
    std::array<event_port, num_event_inputs> m_input_event_ports;
    std::array<event_port, 1> m_output_event_ports;
    inputs_t m_last;
};

template <class F>
event_converter_processor(F &&) -> event_converter_processor<F>;

template <class F>
event_converter_processor(F&&, std::span<std::string_view> const&)
        -> event_converter_processor<F>;

template <class F>
event_converter_processor(
        F&&,
        std::span<std::string_view> const&,
        std::string_view const&) -> event_converter_processor<F>;

template <class F>
event_converter_processor(
        F&&,
        std::span<std::string_view> const&,
        std::string_view const&,
        std::string_view const&) -> event_converter_processor<F>;

} // namespace piejam::audio::engine
