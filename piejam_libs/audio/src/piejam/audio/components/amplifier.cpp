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
        g.add_wire({*m_gain_proc, 0}, {*m_amp_proc, 1});
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
        g.add_wire({*m_gain_proc, 0}, {*m_left_amp_proc, 1});
        g.add_wire({*m_gain_proc, 0}, {*m_right_amp_proc, 1});
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
                  fmt::format("{} gain L", name)))
        , m_right_gain_proc(engine::make_event_to_audio_processor(
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
        g.add_wire({*m_left_gain_proc, 0}, {*m_left_amp_proc, 1});
        g.add_wire({*m_right_gain_proc, 0}, {*m_right_amp_proc, 1});
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
