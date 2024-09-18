// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/mixer_channel.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/components/identity.h>
#include <piejam/audio/components/level_meter.h>
#include <piejam/audio/components/pan_balance.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_algorithms.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/pan_balance_processor.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter_processor_factory.h>

#include <fmt/format.h>

namespace piejam::runtime::components
{

namespace
{

auto
format_name(std::string_view name, std::string_view param)
{
    return fmt::format("{} {}", param, name);
}

class mixer_channel_output final : public audio::engine::component
{
    static auto make_volume_pan_balance_component(
            audio::bus_type bus_type,
            std::string_view name) -> std::unique_ptr<audio::engine::component>
    {
        return bus_type == audio::bus_type::mono
                       ? audio::components::make_pan(
                                 audio::engine::make_volume_pan_processor(name),
                                 name)
                       : audio::components::make_balance(
                                 audio::engine::make_volume_balance_processor(
                                         name),
                                 name);
    }

public:
    mixer_channel_output(
            mixer::channel const& mixer_channel,
            parameter_processor_factory& param_procs,
            audio::sample_rate const sample_rate)
        : m_volume_input_proc(param_procs.make_processor(
                  mixer_channel.volume,
                  format_name(*mixer_channel.name, "volume")))
        , m_pan_balance_input_proc(param_procs.make_processor(
                  mixer_channel.pan_balance,
                  mixer_channel.bus_type == audio::bus_type::mono
                          ? format_name(*mixer_channel.name, "pan")
                          : format_name(*mixer_channel.name, "balance")))
        , m_mute_input_proc(param_procs.make_processor(
                  mixer_channel.mute,
                  format_name(*mixer_channel.name, "mute")))
        , m_volume_pan_balance{make_volume_pan_balance_component(
                  mixer_channel.bus_type,
                  *mixer_channel.name)}
        , m_mute_solo(components::make_mute_solo(*mixer_channel.name))
        , m_level_meter(audio::components::make_stereo_level_meter(sample_rate))
        , m_peak_level_proc(param_procs.make_processor(
                  mixer_channel.peak_level,
                  format_name(*mixer_channel.name, "peak_level")))
        , m_rms_level_proc(param_procs.make_processor(
                  mixer_channel.rms_level,
                  format_name(*mixer_channel.name, "rms_level")))
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_volume_pan_balance->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_mute_solo->outputs();
    }

    auto event_inputs() const -> endpoints override
    {
        return m_event_inputs;
    }

    auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(audio::engine::graph& g) const override
    {
        m_volume_pan_balance->connect(g);
        m_mute_solo->connect(g);
        m_level_meter->connect(g);

        using namespace audio::engine::endpoint_ports;

        audio::engine::connect_event(
                g,
                *m_volume_input_proc,
                from<0>,
                *m_volume_pan_balance,
                to<0>);

        audio::engine::connect_event(
                g,
                *m_pan_balance_input_proc,
                from<0>,
                *m_volume_pan_balance,
                to<1>);

        audio::engine::connect_event(
                g,
                *m_mute_input_proc,
                from<0>,
                *m_mute_solo,
                to<0>);

        audio::engine::connect(g, *m_volume_pan_balance, *m_mute_solo);
        audio::engine::connect(g, *m_volume_pan_balance, *m_level_meter);

        audio::engine::connect_event(
                g,
                *m_level_meter,
                from<0>,
                *m_peak_level_proc,
                to<0>);

        audio::engine::connect_event(
                g,
                *m_level_meter,
                from<1>,
                *m_rms_level_proc,
                to<0>);
    }

private:
    std::shared_ptr<audio::engine::value_io_processor<float>>
            m_volume_input_proc;
    std::shared_ptr<audio::engine::value_io_processor<float>>
            m_pan_balance_input_proc;
    std::shared_ptr<audio::engine::value_io_processor<bool>> m_mute_input_proc;
    std::unique_ptr<audio::engine::component> m_volume_pan_balance;
    std::unique_ptr<audio::engine::component> m_mute_solo;
    std::unique_ptr<audio::engine::component> m_level_meter;
    std::shared_ptr<audio::engine::value_io_processor<stereo_level>>
            m_peak_level_proc;
    std::shared_ptr<audio::engine::value_io_processor<stereo_level>>
            m_rms_level_proc;

    std::array<audio::engine::graph_endpoint, 1> m_event_inputs{
            {m_mute_solo->event_inputs()[1]}};
};

class mixer_channel_aux_send final : public audio::engine::component
{
public:
    mixer_channel_aux_send(
            mixer::channel const& mixer_channel,
            float_parameter_id aux_volume,
            parameter_processor_factory& param_procs)
        : m_volume_param(param_procs.make_processor(
                  aux_volume,
                  format_name(*mixer_channel.name, "aux_volume")))
        , m_volume_amp{audio::components::make_stereo_amplifier(
                  format_name(*mixer_channel.name, "aux_amp"))}
    {
    }
    auto inputs() const -> endpoints override
    {
        return m_volume_amp->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_volume_amp->outputs();
    }

    auto event_inputs() const -> endpoints override
    {
        return {};
    }

    auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(audio::engine::graph& g) const override
    {
        m_volume_amp->connect(g);

        using namespace audio::engine::endpoint_ports;

        audio::engine::connect_event(
                g,
                *m_volume_param,
                from<0>,
                *m_volume_amp,
                to<0>);
    }

private:
    std::shared_ptr<audio::engine::value_io_processor<float>> m_volume_param;
    std::unique_ptr<audio::engine::component> m_volume_amp;
};

} // namespace

auto
make_mixer_channel_input(mixer::channel const& mixer_channel)
        -> std::unique_ptr<audio::engine::component>
{
    return audio::components::make_identity(
            audio::num_channels(mixer_channel.bus_type));
}

auto
make_mixer_channel_output(
        mixer::channel const& mixer_channel,
        parameter_processor_factory& param_procs,
        audio::sample_rate const sample_rate)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<mixer_channel_output>(
            mixer_channel,
            param_procs,
            sample_rate);
}

auto
make_mixer_channel_aux_send(
        mixer::channel const& mixer_channel,
        float_parameter_id aux_volume,
        parameter_processor_factory& param_procs)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<mixer_channel_aux_send>(
            mixer_channel,
            aux_volume,
            param_procs);
}

} // namespace piejam::runtime::components
