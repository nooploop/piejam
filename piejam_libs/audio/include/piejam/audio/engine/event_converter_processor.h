// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/lockstep_events.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/tuple.h>

#include <fmt/format.h>

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include <tuple>

namespace piejam::audio::engine
{

template <class OutputGenerator>
class event_converter_processor final : public named_processor
{
    template <class T>
    struct get_outputs_t
    {
        using type = std::tuple<T>;
    };

    template <class... T>
    struct get_outputs_t<std::tuple<T...>>
    {
        using type = tuple::decay_elements_t<std::tuple<T...>>;
    };

    using outputs_t = typename get_outputs_t<std::decay_t<
            boost::callable_traits::return_type_t<OutputGenerator>>>::type;
    using inputs_t = tuple::decay_elements_t<
            boost::callable_traits::args_t<OutputGenerator>>;

    static constexpr std::size_t num_event_inputs{std::tuple_size_v<inputs_t>};
    static constexpr std::size_t num_event_outputs{
            std::tuple_size_v<outputs_t>};

    static_assert(num_event_inputs > 0);

    static constexpr auto input_index_sequence{
            std::make_index_sequence<num_event_inputs>{}};
    static constexpr auto output_index_sequence{
            std::make_index_sequence<num_event_outputs>{}};

public:
    template <class F>
    event_converter_processor(
            F&& out_gen,
            std::span<std::string_view const> const input_names = {},
            std::span<std::string_view const> const output_names = {},
            std::string_view const name = {})
        : named_processor(name)
        , m_out_gen(std::forward<F>(out_gen))
        , m_input_event_ports(
                  make_event_ports<inputs_t>(input_names, input_index_sequence))
        , m_output_event_ports(make_event_ports<outputs_t>(
                  output_names,
                  output_index_sequence))
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "convert";
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
        return m_input_event_ports;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        return m_output_event_ports;
    }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        process_events(ctx);
    }

private:
    template <class... T>
    void send_result(
            std::tuple<event_buffer<T>&...> const& out_bufs,
            std::size_t offset,
            std::tuple<T...> result)
    {
        std::apply(
                [&, offset](event_buffer<T>&... out_buf) {
                    std::apply(
                            [&, offset](T const&... v) {
                                (out_buf.insert(offset, v), ...);
                            },
                            result);
                },
                out_bufs);
    }

    void process_events(process_context const& ctx)
    {
        auto in_bufs = get_input_buffers(ctx, input_index_sequence);
        auto out_bufs = get_output_buffers(ctx, output_index_sequence);

        m_last = lockstep_events(
                [&]<class... Args>(std::size_t offset, Args&&... args) {
                    send_result(
                            out_bufs,
                            offset,
                            std::tuple{std::invoke(
                                    m_out_gen,
                                    std::forward<Args>(args)...)});
                },
                m_last,
                in_bufs);
    }

    template <class TypesTuple, std::size_t... I>
    static auto make_event_ports(
            std::span<std::string_view const> const names,
            std::index_sequence<I...>)
    {
        return std::array{event_port{
                std::in_place_type<std::tuple_element_t<I, TypesTuple>>,
                I < names.size() ? names[I] : std::string_view{}}...};
    }

    template <std::size_t... I>
    static auto
    get_input_buffers(process_context const& ctx, std::index_sequence<I...>)
    {
        return std::forward_as_tuple(
                ctx.event_inputs.get<std::tuple_element_t<I, inputs_t>>(I)...);
    }

    template <std::size_t... I>
    static auto
    get_output_buffers(process_context const& ctx, std::index_sequence<I...>)
    {
        return std::forward_as_tuple(
                ctx.event_outputs.get<std::tuple_element_t<I, outputs_t>>(
                        I)...);
    }

    OutputGenerator m_out_gen;
    std::array<event_port, num_event_inputs> m_input_event_ports;
    std::array<event_port, num_event_outputs> m_output_event_ports;
    inputs_t m_last;
};

template <class F>
event_converter_processor(F&&) -> event_converter_processor<F>;

template <class F>
event_converter_processor(F&&, std::span<std::string_view const>)
        -> event_converter_processor<F>;

template <class F>
event_converter_processor(
        F&&,
        std::span<std::string_view const>,
        std::span<std::string_view const>) -> event_converter_processor<F>;

template <class F>
event_converter_processor(
        F&&,
        std::span<std::string_view const>,
        std::span<std::string_view const>,
        std::string_view) -> event_converter_processor<F>;

template <class F>
auto
make_event_converter_processor(
        F&& f,
        std::span<std::string_view const> const input_names = {},
        std::span<std::string_view const> const output_name = {},
        std::string_view const name = {}) -> std::unique_ptr<processor>
{
    return std::make_unique<event_converter_processor<F>>(
            std::forward<F>(f),
            input_names,
            output_name,
            name);
}

} // namespace piejam::audio::engine
