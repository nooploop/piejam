// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/audio_engine_middleware.h>

#include <piejam/algorithm/find_or_get_first.h>
#include <piejam/algorithm/for_each_visit.h>
#include <piejam/algorithm/index_of.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/device.h>
#include <piejam/audio/device_manager.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/ladspa/plugin.h>
#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/audio/pcm_io_config.h>
#include <piejam/midi/event.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/runtime/actions/activate_midi_device.h>
#include <piejam/runtime/actions/add_bus.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/control_midi_assignment.h>
#include <piejam/runtime/actions/deactivate_midi_device.h>
#include <piejam/runtime/actions/delete_bus.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/actions/move_fx_module.h>
#include <piejam/runtime/actions/recording.h>
#include <piejam/runtime/actions/request_parameters_update.h>
#include <piejam/runtime/actions/request_streams_update.h>
#include <piejam/runtime/actions/select_bus_channel.h>
#include <piejam/runtime/actions/select_period_count.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_sample_rate.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/actions/update_parameter_values.h>
#include <piejam/runtime/actions/update_streams.h>
#include <piejam/runtime/audio_engine.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/ladspa_manager.h>
#include <piejam/runtime/midi_input_controller.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/thread/worker.h>
#include <piejam/tuple_element_compare.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>
#include <boost/mp11/tuple.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace piejam::runtime
{

namespace
{

constexpr void
update_channel(std::size_t& ch, std::size_t const num_chs)
{
    if (ch >= num_chs)
        ch = npos;
}

struct update_devices final : ui::cloneable_action<update_devices, action>
{
    box<piejam::audio::pcm_io_descriptors> pcm_devices;

    selected_device input;
    selected_device output;

    audio::sample_rate_t sample_rate{};
    audio::period_size_t period_size{};
    audio::period_count_t period_count{};

    auto reduce(state const& st) const -> state override
    {
        auto new_st = st;

        new_st.pcm_devices = pcm_devices;
        new_st.input = input;
        new_st.output = output;
        new_st.sample_rate = sample_rate;
        new_st.period_size = period_size;
        new_st.period_count = period_count;

        new_st.device_io_state.buses.update(
                *st.device_io_state.inputs,
                [num_in_channels = input.hw_params->num_channels](
                        device_io::bus_id,
                        device_io::bus& bus) {
                    update_channel(bus.channels.left, num_in_channels);
                    update_channel(bus.channels.right, num_in_channels);
                });

        new_st.device_io_state.buses.update(
                *st.device_io_state.outputs,
                [num_out_channels = output.hw_params->num_channels](
                        device_io::bus_id,
                        device_io::bus& bus) {
                    update_channel(bus.channels.left, num_out_channels);
                    update_channel(bus.channels.right, num_out_channels);
                });

        return new_st;
    }
};

struct update_info final : ui::cloneable_action<update_info, action>
{
    std::size_t xruns{};
    float cpu_load{};

    auto reduce(state const& st) const -> state override
    {
        auto new_st = st;
        new_st.xruns = xruns;
        new_st.cpu_load = cpu_load;
        return new_st;
    }
};

} // namespace

audio_engine_middleware::audio_engine_middleware(
        middleware_functors mw_fs,
        thread::configuration const& audio_thread_config,
        std::span<thread::configuration const> const& wt_configs,
        audio::device_manager& device_manager,
        fx::ladspa_processor_factory ladspa_fx_processor_factory,
        std::unique_ptr<midi_input_controller> midi_controller)
    : middleware_functors(std::move(mw_fs))
    , m_audio_thread_config(audio_thread_config)
    , m_workers(wt_configs.begin(), wt_configs.end())
    , m_device_manager(device_manager)
    , m_ladspa_fx_processor_factory(std::move(ladspa_fx_processor_factory))
    , m_midi_controller(
              midi_controller ? std::move(midi_controller)
                              : make_dummy_midi_input_controller())
    , m_device(std::make_unique<audio::dummy_device>())
{
}

audio_engine_middleware::~audio_engine_middleware() = default;

void
audio_engine_middleware::operator()(action const& action)
{
    if (auto a = dynamic_cast<actions::device_action const*>(&action))
    {
        if (get_state().recording)
            process_engine_action(actions::stop_recording{});

        close_device();

        auto v = ui::make_action_visitor<actions::device_action_visitor>(
                [this](auto&& a) { process_device_action(a); });

        a->visit(v);

        open_device();
        start_engine();
    }
    else if (auto a = dynamic_cast<actions::engine_action const*>(&action))
    {
        auto v = ui::make_action_visitor<actions::engine_action_visitor>(
                [this](auto&& a) { process_engine_action(a); });

        a->visit(v);
    }
    else
    {
        next(action);
    }
}

template <class Action>
void
audio_engine_middleware::process_device_action(Action const& a)
{
    next(a);
}

static auto
make_update_devices_action(
        audio::device_manager& device_manager,
        box<audio::pcm_io_descriptors> new_devices,
        box<audio::pcm_io_descriptors> current_devices,
        std::size_t const input_index,
        std::size_t const output_index,
        audio::sample_rate_t const sample_rate,
        audio::period_size_t const period_size,
        audio::period_count_t const period_count) -> update_devices
{
    update_devices next_action;
    next_action.pcm_devices = new_devices;

    auto next_device = [&](auto const& new_devices,
                           auto const& current_devices,
                           auto const current_index) {
        auto const found_index =
                current_index != npos ? algorithm::index_of(
                                                new_devices,
                                                current_devices[current_index])
                                      : npos;
        return selected_device{
                .index = found_index,
                .hw_params = found_index != npos
                                     ? device_manager.hw_params(
                                               new_devices[found_index],
                                               nullptr,
                                               nullptr)
                                     : audio::pcm_hw_params{}};
    };

    next_action.input = next_device(
            new_devices->inputs,
            current_devices->inputs,
            input_index);

    next_action.output = next_device(
            new_devices->outputs,
            current_devices->outputs,
            output_index);

    auto next_value = [](box<audio::pcm_hw_params> input_hw_params,
                         box<audio::pcm_hw_params> output_hw_params,
                         auto&& sel,
                         auto current) {
        auto const values = std::invoke(
                std::forward<decltype(sel)>(sel),
                input_hw_params,
                output_hw_params);
        auto const it = algorithm::find_or_get_first(values, current);
        return it != values.end() ? *it : 0;
    };

    next_action.sample_rate = next_value(
            next_action.input.hw_params,
            next_action.output.hw_params,
            &sample_rates,
            sample_rate);

    if (next_action.sample_rate != 0)
    {
        next_action.input.hw_params = device_manager.hw_params(
                new_devices->inputs[next_action.input.index],
                &next_action.sample_rate,
                nullptr);
        next_action.output.hw_params = device_manager.hw_params(
                new_devices->outputs[next_action.output.index],
                &next_action.sample_rate,
                nullptr);
    }

    next_action.period_size = next_value(
            next_action.input.hw_params,
            next_action.output.hw_params,
            &period_sizes,
            period_size);

    if (next_action.period_size != 0)
    {
        next_action.input.hw_params = device_manager.hw_params(
                new_devices->inputs[next_action.input.index],
                &next_action.sample_rate,
                &next_action.period_size);
        next_action.output.hw_params = device_manager.hw_params(
                new_devices->outputs[next_action.output.index],
                &next_action.sample_rate,
                &next_action.period_size);
    }

    next_action.period_count = next_value(
            next_action.input.hw_params,
            next_action.output.hw_params,
            &period_counts,
            period_count);

    return next_action;
}

template <>
void
audio_engine_middleware::process_device_action(
        actions::apply_app_config const& a)
{
    state const& current_state = get_state();

    next(make_update_devices_action(
            m_device_manager,
            current_state.pcm_devices,
            current_state.pcm_devices,
            algorithm::index_of_if(
                    current_state.pcm_devices->inputs,
                    [&](auto const& d) {
                        return d.name == a.conf.input_device_name;
                    }),
            algorithm::index_of_if(
                    current_state.pcm_devices->outputs,
                    [&](auto const& d) {
                        return d.name == a.conf.output_device_name;
                    }),
            a.conf.sample_rate,
            a.conf.period_size,
            a.conf.period_count));

    next(a);
}

template <>
void
audio_engine_middleware::process_device_action(actions::refresh_devices const&)
{
    state const& current_state = get_state();

    next(make_update_devices_action(
            m_device_manager,
            m_device_manager.io_descriptors(),
            current_state.pcm_devices,
            current_state.input.index,
            current_state.output.index,
            current_state.sample_rate,
            current_state.period_size,
            current_state.period_count));
}

template <>
void
audio_engine_middleware::process_device_action(
        actions::initiate_device_selection const& action)
{
    state const& current_state = get_state();

    next(make_update_devices_action(
            m_device_manager,
            current_state.pcm_devices,
            current_state.pcm_devices,
            action.input ? action.index : current_state.input.index,
            action.input ? current_state.output.index : action.index,
            current_state.sample_rate,
            current_state.period_size,
            current_state.period_count));
}

template <>
void
audio_engine_middleware::process_device_action(
        actions::select_sample_rate const& action)
{
    state const& current_state = get_state();

    auto const srs = sample_rates_from_state(current_state);
    if (action.index < srs.size())
    {
        next(make_update_devices_action(
                m_device_manager,
                current_state.pcm_devices,
                current_state.pcm_devices,
                current_state.input.index,
                current_state.output.index,
                srs[action.index],
                current_state.period_size,
                current_state.period_count));
    }
}

template <>
void
audio_engine_middleware::process_device_action(
        actions::select_period_size const& action)
{
    state const& current_state = get_state();

    auto const pss = period_sizes_from_state(current_state);
    if (action.index < pss.size())
    {
        next(make_update_devices_action(
                m_device_manager,
                current_state.pcm_devices,
                current_state.pcm_devices,
                current_state.input.index,
                current_state.output.index,
                current_state.sample_rate,
                pss[action.index],
                current_state.period_count));
    }
}

template <>
void
audio_engine_middleware::process_device_action(
        actions::select_period_count const& action)
{
    state const& current_state = get_state();

    auto const pcs = period_counts_from_state(current_state);
    if (action.index < pcs.size())
    {
        next(make_update_devices_action(
                m_device_manager,
                current_state.pcm_devices,
                current_state.pcm_devices,
                current_state.input.index,
                current_state.output.index,
                current_state.sample_rate,
                current_state.period_size,
                pcs[action.index]));
    }
}

template <>
void
audio_engine_middleware::process_device_action(
        actions::activate_midi_device const& action)
{
    if (m_midi_controller->activate_input_device(action.device_id))
        next(action);
}

template <>
void
audio_engine_middleware::process_device_action(
        actions::deactivate_midi_device const& action)
{
    m_midi_controller->deactivate_input_device(action.device_id);
    next(action);
}

template <class Action>
void
audio_engine_middleware::process_engine_action(Action const& a)
{
    next(a);
    rebuild();
}

template <class Parameter>
void
audio_engine_middleware::process_engine_action(
        actions::set_parameter_value<Parameter> const& a)
{
    next(a);

    if (m_engine)
    {
        m_engine->set_parameter_value(
                a.id,
                get_parameter_value(get_state().params, a.id));
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::request_parameters_update const& a)
{
    if (m_engine)
    {
        actions::update_parameter_values next_action;

        boost::mp11::tuple_for_each(
                a.param_ids,
                [this, &next_action](auto&& ids) {
                    for (auto&& id : ids)
                    {
                        if (auto value = m_engine->get_parameter_update(id))
                            next_action.push_back(id, *value);
                    }
                });

        if (!next_action.empty())
            next(next_action);
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::request_streams_update const& a)
{
    if (m_engine)
    {
        actions::update_streams next_action;

        for (auto const& id : a.streams)
        {
            auto const& st = get_state();
            if (audio_stream_buffer const* stream = st.streams.find(id))
            {
                if (auto buffer = m_engine->get_stream(id); !buffer.empty())
                {
                    next_action.streams.emplace(
                            id,
                            audio_stream_buffer(
                                    std::in_place,
                                    std::move(buffer),
                                    stream->get().num_channels()));
                }
            }
        }

        if (!next_action.streams.empty())
            next(next_action);
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::request_info_update const&)
{
    {
        update_info next_action;
        next_action.xruns = m_device->xruns();
        next_action.cpu_load = m_device->cpu_load();

        next(next_action);
    }

    if (m_engine)
    {
        if (auto learned_midi = m_engine->get_learned_midi())
        {
            if (auto const* const cc_event =
                        std::get_if<midi::channel_cc_event>(
                                &learned_midi->event))
            {
                actions::update_midi_assignments next_action;
                next_action.assignments.emplace(
                        *get_state().midi_learning,
                        midi_assignment{
                                .channel = cc_event->channel,
                                .control_type = midi_assignment::type::cc,
                                .control_id = cc_event->data.cc});

                next(next_action);
            }

            next(actions::stop_midi_learning{});

            rebuild();
        }
    }
}

void
audio_engine_middleware::close_device()
{
    m_device = std::make_unique<piejam::audio::dummy_device>();

    // The engine is executed by a device, we can safely destroy it after device
    // was closed.
    m_engine.reset();
}

void
audio_engine_middleware::open_device()
{
    auto const& st = get_state();

    if (st.input.index == npos || st.output.index == npos ||
        st.sample_rate == 0 || st.period_size == 0 || st.period_count == 0)
        return;

    try
    {
        auto device = m_device_manager.make_device(
                st.pcm_devices->inputs[st.input.index],
                st.pcm_devices->outputs[st.output.index],
                audio::pcm_io_config{
                        audio::pcm_device_config{
                                st.input.hw_params->interleaved,
                                st.input.hw_params->format,
                                st.input.hw_params->num_channels},
                        audio::pcm_device_config{
                                st.output.hw_params->interleaved,
                                st.output.hw_params->format,
                                st.output.hw_params->num_channels},
                        audio::pcm_process_config{
                                st.sample_rate,
                                st.period_size,
                                st.period_count}});
        m_device.swap(device);
    }
    catch (std::exception const& err)
    {
        spdlog::error("opening device failed: {}", err.what());
    }
}

void
audio_engine_middleware::start_engine()
{
    BOOST_ASSERT(m_device);

    if (m_device->is_open())
    {
        auto const& state = get_state();

        m_engine = std::make_unique<audio_engine>(
                m_workers,
                state.sample_rate,
                state.input.hw_params->num_channels,
                state.output.hw_params->num_channels);

        m_device->start(
                m_audio_thread_config,
                [engine = m_engine.get()](auto const& in, auto const& out) {
                    engine->init_process(in, out);
                },
                [engine = m_engine.get()](auto const buffer_size) {
                    engine->process(buffer_size);
                });

        rebuild();
    }
}

void
audio_engine_middleware::rebuild()
{
    if (!m_engine || !m_device->is_running())
        return;

    auto const& st = get_state();
    if (!m_engine->rebuild(
                st,
                [this, sr = st.sample_rate](fx::ladspa_instance_id id) {
                    return m_ladspa_fx_processor_factory(id, sr);
                },
                m_midi_controller->make_input_event_handler()))
    {
        spdlog::error("audio_engine_middleware: graph rebuilding failed");
    }
}

} // namespace piejam::runtime
