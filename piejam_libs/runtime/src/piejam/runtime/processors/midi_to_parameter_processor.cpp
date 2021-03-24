// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/midi_to_parameter_processor.h>

#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/midi/event.h>
#include <piejam/range/iota.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>

#include <boost/assert.hpp>

#include <array>
#include <memory>
#include <ranges>

namespace piejam::runtime::processors
{

static auto
make_cc_to_float_lut(float_parameter const& param) -> std::array<float, 128>
{
    std::array<float, 128> result;

    std::ranges::transform(
            range::iota(std::size_t{128}),
            result.begin(),
            [&param](std::size_t const cc_value) -> float {
                return param.from_normalized(param, cc_value / 127.f);
            });

    return result;
}

auto
make_midi_cc_to_parameter_processor(float_parameter const& param)
        -> std::unique_ptr<audio::engine::processor>
{
    static auto const input_name = "cc in";
    static std::array<std::string_view, 1> input_names{input_name};
    return audio::engine::make_event_converter_processor(
            [lut = make_cc_to_float_lut(param)](midi::cc_event const& cc_ev)
                    -> float { return lut[cc_ev.value]; },
            input_names,
            "float",
            "cc_to_float");
}

static auto
make_cc_to_int_lut(int_parameter const& param) -> std::array<int, 128>
{
    std::array<int, 128> result;

    auto const num_values = param.max - param.min + 1;
    if (num_values <= 32)
    {
        std::ranges::transform(
                range::iota(std::size_t{128}),
                result.begin(),
                [min = param.min,
                 max = param.max](std::size_t const cc_value) -> int {
                    return std::min(static_cast<int>(cc_value / 4) + min, max);
                });
    }
    else if (num_values <= 64)
    {
        std::ranges::transform(
                range::iota(std::size_t{128}),
                result.begin(),
                [min = param.min,
                 max = param.max](std::size_t const cc_value) -> int {
                    return std::min(static_cast<int>(cc_value / 2) + min, max);
                });
    }
    else
    {
        std::ranges::transform(
                range::iota(std::size_t{128}),
                result.begin(),
                [min = param.min, diff = param.max - param.min](
                        std::size_t const cc_value) -> int {
                    return static_cast<int>(
                            (cc_value / 127.f) * diff + min + 0.5f);
                });
    }

    return result;
}

auto
make_midi_cc_to_parameter_processor(int_parameter const& param)
        -> std::unique_ptr<audio::engine::processor>
{
    static auto const input_name = "cc in";
    static std::array<std::string_view, 1> input_names{input_name};

    return audio::engine::make_event_converter_processor(
            [lut = make_cc_to_int_lut(param)](
                    midi::cc_event const& cc_ev) -> int {
                BOOST_ASSERT(cc_ev.value < 128);
                return lut[cc_ev.value];
            },
            input_names,
            "int",
            "cc_to_int");
}

auto
make_midi_cc_to_parameter_processor(bool_parameter const&)
        -> std::unique_ptr<audio::engine::processor>
{
    static auto const input_name = "cc in";
    static std::array<std::string_view, 1> input_names{input_name};
    return audio::engine::make_event_converter_processor(
            [](midi::cc_event const& cc_ev) -> bool {
                return cc_ev.value != 0;
            },
            input_names,
            "bool",
            "cc_to_bool");
}

} // namespace piejam::runtime::processors
