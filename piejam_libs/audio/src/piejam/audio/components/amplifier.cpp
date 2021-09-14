// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/amplifier.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_to_audio_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/multiply_processor.h>
#include <piejam/audio/engine/processor.h>

#include <fmt/format.h>

#include <array>

namespace piejam::audio::components
{

namespace
{

class mono_amplifier final : public engine::component
{
public:
    mono_amplifier(std::string_view name)
        : m_gain_proc(engine::make_event_to_audio_processor(
                  engine::default_event_to_audio_smooth_length,
                  fmt::format("{} gain", name)))
        , m_amp_proc(engine::make_multiply_processor(2, name))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(engine::graph& g) const override
    {
        g.audio.insert({*m_gain_proc, 0}, {*m_amp_proc, 1});
    }

private:
    std::unique_ptr<engine::processor> m_gain_proc;
    std::unique_ptr<engine::processor> m_amp_proc;

    std::array<engine::graph_endpoint, 1> m_inputs{{{*m_amp_proc, 0}}};
    std::array<engine::graph_endpoint, 1> m_outputs{{{*m_amp_proc, 0}}};
    std::array<engine::graph_endpoint, 1> m_event_inputs{{{*m_gain_proc, 0}}};
};

class stereo_amplifier final : public engine::component
{
public:
    stereo_amplifier(std::string_view name)
        : m_gain_proc(engine::make_event_to_audio_processor(
                  engine::default_event_to_audio_smooth_length,
                  fmt::format("{} gain", name)))
        , m_left_amp_proc(engine::make_multiply_processor(
                  2,
                  fmt::format("{} amp L", name)))
        , m_right_amp_proc(engine::make_multiply_processor(
                  2,
                  fmt::format("{} amp R", name)))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(engine::graph& g) const override
    {
        g.audio.insert({*m_gain_proc, 0}, {*m_left_amp_proc, 1});
        g.audio.insert({*m_gain_proc, 0}, {*m_right_amp_proc, 1});
    }

private:
    std::unique_ptr<engine::processor> m_gain_proc;
    std::unique_ptr<engine::processor> m_left_amp_proc;
    std::unique_ptr<engine::processor> m_right_amp_proc;

    std::array<engine::graph_endpoint, 2> m_inputs{
            {{*m_left_amp_proc, 0}, {*m_right_amp_proc, 0}}};
    std::array<engine::graph_endpoint, 2> m_outputs{
            {{*m_left_amp_proc, 0}, {*m_right_amp_proc, 0}}};
    std::array<engine::graph_endpoint, 1> m_event_inputs{{{*m_gain_proc, 0}}};
};

class stereo_split_amplifier final : public engine::component
{
public:
    stereo_split_amplifier(std::string_view name)
        : m_left_gain_proc(engine::make_event_to_audio_processor(
                  engine::default_event_to_audio_smooth_length,
                  fmt::format("{} gain L", name)))
        , m_right_gain_proc(engine::make_event_to_audio_processor(
                  engine::default_event_to_audio_smooth_length,
                  fmt::format("{} gain R", name)))
        , m_left_amp_proc(engine::make_multiply_processor(
                  2,
                  fmt::format("{} amp L", name)))
        , m_right_amp_proc(engine::make_multiply_processor(
                  2,
                  fmt::format("{} amp R", name)))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(engine::graph& g) const override
    {
        g.audio.insert({*m_left_gain_proc, 0}, {*m_left_amp_proc, 1});
        g.audio.insert({*m_right_gain_proc, 0}, {*m_right_amp_proc, 1});
    }

private:
    std::unique_ptr<engine::processor> m_left_gain_proc;
    std::unique_ptr<engine::processor> m_right_gain_proc;
    std::unique_ptr<engine::processor> m_left_amp_proc;
    std::unique_ptr<engine::processor> m_right_amp_proc;

    std::array<engine::graph_endpoint, 2> m_inputs{
            {{*m_left_amp_proc, 0}, {*m_right_amp_proc, 0}}};
    std::array<engine::graph_endpoint, 2> m_outputs{
            {{*m_left_amp_proc, 0}, {*m_right_amp_proc, 0}}};
    std::array<engine::graph_endpoint, 2> m_event_inputs{
            {{*m_left_gain_proc, 0}, {*m_right_gain_proc, 0}}};
};

} // namespace

auto
make_mono_amplifier(std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<mono_amplifier>(name);
}

auto
make_stereo_amplifier(std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return std::make_unique<stereo_amplifier>(name);
}

auto
make_stereo_split_amplifier(std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return std::make_unique<stereo_split_amplifier>(name);
}

} // namespace piejam::audio::components
