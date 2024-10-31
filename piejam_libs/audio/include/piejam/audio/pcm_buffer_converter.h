// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>
#include <functional>
#include <span>

namespace piejam::audio
{

// from pcm to target
class pcm_input_buffer_converter
{
public:
    pcm_input_buffer_converter() noexcept = default;

    template <std::invocable<std::span<float>> F>
    explicit pcm_input_buffer_converter(F&& fn)
        : m_converter(std::forward<F>(fn))
    {
    }

    void operator()(std::span<float> target_buffer) const
    {
        m_converter(target_buffer);
    }

private:
    using converter_fn =
            std::function<void(std::span<float> /* target_buffer */)>;

    converter_fn m_converter{[](auto...) {}};
};

// from source to pcm
class pcm_output_buffer_converter
{
public:
    pcm_output_buffer_converter() noexcept = default;

    template <
            std::invocable<float, std::size_t> CF,
            std::invocable<std::span<float const>> SF>
    explicit pcm_output_buffer_converter(CF&& cf, SF&& sf)
        : m_constant_converter{std::forward<CF>(cf)}
        , m_span_converter{std::forward<SF>(sf)}
    {
    }

    void operator()(float constant, std::size_t size) const
    {
        m_constant_converter(constant, size);
    }

    void operator()(std::span<float const> source_buffer) const
    {
        m_span_converter(source_buffer);
    }

private:
    using constant_converter_fn =
            std::function<void(float /* constant */, std::size_t /* size */)>;
    using span_converter_fn =
            std::function<void(std::span<float const> /* source_buffer */)>;

    constant_converter_fn m_constant_converter{[](auto...) {}};
    span_converter_fn m_span_converter{[](auto...) {}};
};

} // namespace piejam::audio
