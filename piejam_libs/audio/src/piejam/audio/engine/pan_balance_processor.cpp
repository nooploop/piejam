// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/pan_balance_processor.h>

#include <piejam/audio/dsp/pan.h>
#include <piejam/audio/engine/event_converter_processor.h>

#include <array>

namespace piejam::audio::engine
{

auto
make_pan_processor(std::string_view const name)
        -> std::unique_ptr<audio::engine::processor>
{
    using namespace std::string_view_literals;
    static constexpr std::array s_input_names{"pan"sv};
    static constexpr std::array s_output_names{"gain L"sv, "gain R"sv};
    return make_event_converter_processor(
            [](float param) -> std::tuple<float, float> {
                auto stereo_gain = dsp::sinusoidal_constant_power_pan(param);
                return std::tuple{stereo_gain.left, stereo_gain.right};
            },
            s_input_names,
            s_output_names,
            fmt::format("pan {}", name));
}

auto
make_volume_pan_processor(std::string_view const name)
        -> std::unique_ptr<audio::engine::processor>
{
    using namespace std::string_view_literals;
    static constexpr std::array s_input_names{"volume"sv, "pan"sv};
    static constexpr std::array s_output_names{"gain L"sv, "gain R"sv};
    return make_event_converter_processor(
            [](float volume, float pan) -> std::tuple<float, float> {
                auto pan_gain = dsp::sinusoidal_constant_power_pan(pan);
                return std::tuple{
                        volume * pan_gain.left,
                        volume * pan_gain.right};
            },
            s_input_names,
            s_output_names,
            fmt::format("volume_pan {}", name));
}

auto
make_balance_processor(std::string_view const name)
        -> std::unique_ptr<audio::engine::processor>
{
    using namespace std::string_view_literals;
    static constexpr std::array s_input_names{"balance"sv};
    static constexpr std::array s_output_names{"gain L"sv, "gain R"sv};
    return make_event_converter_processor(
            [](float param) -> std::tuple<float, float> {
                auto stereo_gain = dsp::stereo_balance(param);
                return std::tuple{stereo_gain.left, stereo_gain.right};
            },
            s_input_names,
            s_output_names,
            fmt::format("balance {}", name));
}

auto
make_volume_balance_processor(std::string_view const name)
        -> std::unique_ptr<audio::engine::processor>
{
    using namespace std::string_view_literals;
    static constexpr std::array s_input_names{"volume"sv, "balance"sv};
    static constexpr std::array s_output_names{"gain L"sv, "gain R"sv};
    return make_event_converter_processor(
            [](float volume, float param) -> std::tuple<float, float> {
                auto balance_gain = dsp::stereo_balance(param);
                return std::tuple{
                        volume * balance_gain.left,
                        volume * balance_gain.right};
            },
            s_input_names,
            s_output_names,
            fmt::format("volume_balance {}", name));
}

} // namespace piejam::audio::engine
