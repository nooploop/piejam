// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/level_meter.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/level_meter_processor.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/pair.h>
#include <piejam/audio/sample_rate.h>

#include <fmt/format.h>

#include <array>

namespace piejam::audio::components
{

namespace
{

class stereo_level_meter final : public engine::component
{
public:
    stereo_level_meter(sample_rate const sample_rate, std::string_view name)
        : m_left_peak_lm_proc(engine::make_peak_level_meter_processor(
                  sample_rate,
                  fmt::format("{} peak L", name)))
        , m_left_rms_lm_proc(engine::make_rms_level_meter_processor(
                  sample_rate,
                  fmt::format("{} rms L", name)))
        , m_right_peak_lm_proc(engine::make_peak_level_meter_processor(
                  sample_rate,
                  fmt::format("{} peak R", name)))
        , m_right_rms_lm_proc(engine::make_rms_level_meter_processor(
                  sample_rate,
                  fmt::format("{} rms R", name)))
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    auto outputs() const -> endpoints override
    {
        return {};
    }

    auto event_inputs() const -> endpoints override
    {
        return {};
    }

    auto event_outputs() const -> endpoints override
    {
        return m_event_outputs;
    }

    void connect(engine::graph& g) const override
    {
        g.audio.insert({*m_left_input_proc, 0}, {*m_left_peak_lm_proc, 0});
        g.audio.insert({*m_left_input_proc, 0}, {*m_left_rms_lm_proc, 0});

        g.audio.insert({*m_right_input_proc, 0}, {*m_right_peak_lm_proc, 0});
        g.audio.insert({*m_right_input_proc, 0}, {*m_right_rms_lm_proc, 0});

        g.event.insert(
                {*m_left_peak_lm_proc, 0},
                {*m_peak_level_convert_proc, 0});
        g.event.insert(
                {*m_right_peak_lm_proc, 0},
                {*m_peak_level_convert_proc, 1});

        g.event.insert(
                {*m_left_rms_lm_proc, 0},
                {*m_rms_level_convert_proc, 0});
        g.event.insert(
                {*m_right_rms_lm_proc, 0},
                {*m_rms_level_convert_proc, 1});
    }

private:
    static auto make_stereo_level_converter(std::string_view name)
            -> std::unique_ptr<engine::processor>
    {
        using namespace std::string_view_literals;
        static constexpr std::array s_input_names{"level L"sv, "level R"sv};
        static constexpr std::array s_output_names{"stereo_level"sv};

        return std::unique_ptr<engine::processor>{
                new engine::event_converter_processor(
                        [](float l, float r) -> pair<float> { return {l, r}; },
                        s_input_names,
                        s_output_names,
                        name)};
    }

    std::unique_ptr<engine::processor> m_left_input_proc{
            engine::make_identity_processor()};
    std::unique_ptr<engine::processor> m_right_input_proc{
            engine::make_identity_processor()};
    std::unique_ptr<engine::processor> m_left_peak_lm_proc;
    std::unique_ptr<engine::processor> m_left_rms_lm_proc;
    std::unique_ptr<engine::processor> m_right_peak_lm_proc;
    std::unique_ptr<engine::processor> m_right_rms_lm_proc;
    std::unique_ptr<engine::processor> m_peak_level_convert_proc{
            make_stereo_level_converter("peak_level")};
    std::unique_ptr<engine::processor> m_rms_level_convert_proc{
            make_stereo_level_converter("rms_level")};

    std::array<engine::graph_endpoint, 2> m_inputs{{
            {*m_left_input_proc, 0},
            {*m_right_input_proc, 0},
    }};
    std::array<engine::graph_endpoint, 2> m_event_outputs{{
            {*m_peak_level_convert_proc, 0},
            {*m_rms_level_convert_proc, 0},
    }};
};

} // namespace

auto
make_stereo_level_meter(sample_rate const sample_rate, std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return std::make_unique<stereo_level_meter>(sample_rate, name);
}

} // namespace piejam::audio::components
