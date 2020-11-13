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

#include <piejam/audio/components/level_meter.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/level_meter_processor.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/pair.h>

#include <fmt/format.h>

#include <array>

namespace piejam::audio::components
{

namespace
{

class stereo_level_meter final : public engine::component
{
public:
    stereo_level_meter(samplerate_t samplerate, std::string_view name)
        : m_left_lm_proc(std::make_unique<engine::level_meter_processor>(
                  samplerate,
                  fmt::format("{} L", name)))
        , m_right_lm_proc(std::make_unique<engine::level_meter_processor>(
                  samplerate,
                  fmt::format("{} R", name)))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return {}; }

    auto event_inputs() const -> endpoints override { return {}; }
    auto event_outputs() const -> endpoints override { return m_event_outputs; }

    void connect(engine::graph& g) override
    {
        g.add_event_wire({*m_left_lm_proc, 0}, {*m_level_convert_proc, 0});
        g.add_event_wire({*m_right_lm_proc, 0}, {*m_level_convert_proc, 1});
    }

private:
    static auto make_stereo_level_converter()
            -> std::unique_ptr<engine::processor>
    {
        static std::array const s_port_names{
                std::string_view("level l"),
                std::string_view("level r")};
        return std::unique_ptr<engine::processor>{
                new engine::event_converter_processor(
                        [](float l, float r) -> pair<float> {
                            return {l, r};
                        },
                        std::span(s_port_names),
                        "stereo_level")};
    }

    std::unique_ptr<engine::processor> m_left_lm_proc;
    std::unique_ptr<engine::processor> m_right_lm_proc;
    std::unique_ptr<engine::processor> m_level_convert_proc{
            make_stereo_level_converter()};

    std::array<engine::graph_endpoint, 2> m_inputs{
            {{*m_left_lm_proc, 0}, {*m_right_lm_proc, 0}}};
    std::array<engine::graph_endpoint, 1> m_event_outputs{
            {{*m_level_convert_proc, 0}}};
};

} // namespace

auto
make_stereo_level_meter(samplerate_t samplerate, std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return std::make_unique<stereo_level_meter>(samplerate, name);
}

} // namespace piejam::audio::components
