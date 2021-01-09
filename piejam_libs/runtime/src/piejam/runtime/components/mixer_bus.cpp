// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/mixer_bus.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/components/level_meter.h>
#include <piejam/audio/components/pan.h>
#include <piejam/audio/components/stereo_balance.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_algorithms.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/components/mute_solo.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter_processor_factory.h>

namespace piejam::runtime::components
{

namespace
{

class mixer_bus_input final : public audio::engine::component
{
public:
    mixer_bus_input(
            mixer::bus const& bus,
            parameter_processor_factory& param_procs)
        : m_pan_balance_input_proc(param_procs.make_input_processor(
                  bus.pan_balance,
                  bus.type == audio::bus_type::mono ? "pan" : "balance"))
        , m_pan_balance(
                  bus.type == audio::bus_type::mono
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

    auto event_inputs() const -> endpoints override { return {}; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) const override
    {
        m_pan_balance->connect(g);

        g.add_event_wire(
                {*m_pan_balance_input_proc, 0},
                m_pan_balance->event_inputs()[0]);
    }

private:
    std::shared_ptr<audio::engine::value_input_processor<float>>
            m_pan_balance_input_proc;
    std::unique_ptr<audio::engine::component> m_pan_balance;
};

class mixer_bus_output final : public audio::engine::component
{
public:
    mixer_bus_output(
            audio::samplerate_t const samplerate,
            mixer::bus_id bus_id,
            mixer::bus const& bus,
            parameter_processor_factory& param_procs)
        : m_volume_input_proc(
                  param_procs.make_input_processor(bus.volume, "volume"))
        , m_mute_input_proc(param_procs.make_input_processor(bus.mute, "mute"))
        , m_volume_amp(audio::components::make_stereo_amplifier("volume"))
        , m_mute_solo(components::make_mute_solo(bus_id))
        , m_level_meter(audio::components::make_stereo_level_meter(samplerate))
        , m_peak_level_proc(
                  param_procs.make_output_processor(bus.level, "stereo_level"))
    {
    }

    auto inputs() const -> endpoints override { return m_volume_amp->inputs(); }
    auto outputs() const -> endpoints override
    {
        return m_mute_solo->outputs();
    }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) const override
    {
        m_volume_amp->connect(g);
        m_mute_solo->connect(g);
        m_level_meter->connect(g);

        g.add_event_wire(
                {*m_volume_input_proc, 0},
                m_volume_amp->event_inputs()[0]);
        g.add_event_wire(
                {*m_mute_input_proc, 0},
                m_mute_solo->event_inputs()[0]);

        audio::engine::connect_stereo_components(
                g,
                *m_volume_amp,
                *m_mute_solo);
        audio::engine::connect_stereo_components(
                g,
                *m_volume_amp,
                *m_level_meter);

        g.add_event_wire(
                m_level_meter->event_outputs()[0],
                {*m_peak_level_proc, 0});
    }

private:
    std::shared_ptr<audio::engine::value_input_processor<float>>
            m_volume_input_proc;
    std::shared_ptr<audio::engine::value_input_processor<bool>>
            m_mute_input_proc;
    std::unique_ptr<audio::engine::component> m_volume_amp;
    std::unique_ptr<audio::engine::component> m_mute_solo;
    std::unique_ptr<audio::engine::component> m_level_meter;
    std::shared_ptr<audio::engine::value_output_processor<stereo_level>>
            m_peak_level_proc;

    std::array<audio::engine::graph_endpoint, 1> m_event_inputs{
            {m_mute_solo->event_inputs()[1]}};
};

} // namespace

auto
make_mixer_bus_input(
        mixer::bus const& bus,
        parameter_processor_factory& param_procs,
        std::string_view const& /*name*/)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<mixer_bus_input>(bus, param_procs);
}

auto
make_mixer_bus_output(
        audio::samplerate_t const samplerate,
        mixer::bus_id bus_id,
        mixer::bus const& channel,
        parameter_processor_factory& param_procs,
        std::string_view const& /*name*/)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<mixer_bus_output>(
            samplerate,
            bus_id,
            channel,
            param_procs);
}

} // namespace piejam::runtime::components
