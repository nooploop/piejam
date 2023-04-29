// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/mixer_channel.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/components/level_meter.h>
#include <piejam/audio/components/pan_balance.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_algorithms.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter_processor_factory.h>

namespace piejam::runtime::components
{

namespace
{

class mixer_channel_input final : public audio::engine::component
{
public:
    mixer_channel_input(
            mixer::channel const& mixer_channel,
            audio::bus_type const bus_type,
            parameter_processor_factory& param_procs)
        : m_pan_balance_input_proc(param_procs.make_processor(
                  mixer_channel.pan_balance,
                  bus_type == audio::bus_type::mono ? "pan" : "balance"))
        , m_pan_balance(
                  bus_type == audio::bus_type::mono
                          ? audio::components::make_pan()
                          : audio::components::make_stereo_balance())
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_pan_balance->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_pan_balance->outputs();
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
        m_pan_balance->connect(g);

        g.event.insert(
                {*m_pan_balance_input_proc, 0},
                m_pan_balance->event_inputs()[0]);
    }

private:
    std::shared_ptr<audio::engine::value_io_processor<float>>
            m_pan_balance_input_proc;
    std::unique_ptr<audio::engine::component> m_pan_balance;
};

class mixer_channel_output final : public audio::engine::component
{
public:
    mixer_channel_output(
            audio::sample_rate const sample_rate,
            mixer::channel const& mixer_channel,
            parameter_processor_factory& param_procs)
        : m_volume_input_proc(
                  param_procs.make_processor(mixer_channel.volume, "volume"))
        , m_mute_input_proc(
                  param_procs.make_processor(mixer_channel.mute, "mute"))
        , m_volume_amp(audio::components::make_stereo_amplifier("volume"))
        , m_mute_solo(components::make_mute_solo())
        , m_level_meter(audio::components::make_stereo_level_meter(sample_rate))
        , m_peak_level_proc(param_procs.make_processor(
                  mixer_channel.level,
                  "stereo_level"))
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_volume_amp->inputs();
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
        m_volume_amp->connect(g);
        m_mute_solo->connect(g);
        m_level_meter->connect(g);

        g.event.insert(
                {*m_volume_input_proc, 0},
                m_volume_amp->event_inputs()[0]);
        g.event.insert({*m_mute_input_proc, 0}, m_mute_solo->event_inputs()[0]);

        audio::engine::connect_stereo_components(
                g,
                *m_volume_amp,
                *m_mute_solo);
        audio::engine::connect_stereo_components(
                g,
                *m_volume_amp,
                *m_level_meter);

        g.event.insert(
                m_level_meter->event_outputs()[0],
                {*m_peak_level_proc, 0});
    }

private:
    std::shared_ptr<audio::engine::value_io_processor<float>>
            m_volume_input_proc;
    std::shared_ptr<audio::engine::value_io_processor<bool>> m_mute_input_proc;
    std::unique_ptr<audio::engine::component> m_volume_amp;
    std::unique_ptr<audio::engine::component> m_mute_solo;
    std::unique_ptr<audio::engine::component> m_level_meter;
    std::shared_ptr<audio::engine::value_io_processor<stereo_level>>
            m_peak_level_proc;

    std::array<audio::engine::graph_endpoint, 1> m_event_inputs{
            {m_mute_solo->event_inputs()[1]}};
};

} // namespace

auto
make_mixer_channel_input(
        mixer::channel const& mixer_channel,
        audio::bus_type const bus_type,
        parameter_processor_factory& param_procs,
        std::string_view const /*name*/)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<mixer_channel_input>(
            mixer_channel,
            bus_type,
            param_procs);
}

auto
make_mixer_channel_output(
        mixer::channel const& mixer_channel,
        audio::sample_rate const sample_rate,
        parameter_processor_factory& param_procs,
        std::string_view const /*name*/)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<mixer_channel_output>(
            sample_rate,
            mixer_channel,
            param_procs);
}

} // namespace piejam::runtime::components
