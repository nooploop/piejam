// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/audio_engine_middleware.h>

#include <piejam/runtime/actions/activate_midi_device.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/apply_session.h>
#include <piejam/runtime/actions/audio_engine_sync.h>
#include <piejam/runtime/actions/control_midi_assignment.h>
#include <piejam/runtime/actions/deactivate_midi_device.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/external_audio_device_actions.h>
#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/actions/initiate_sound_card_selection.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/actions/move_fx_module.h>
#include <piejam/runtime/actions/recording.h>
#include <piejam/runtime/actions/select_period_count.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_sample_rate.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/audio_engine.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/midi_input_controller.h>
#include <piejam/runtime/state.h>

#include <piejam/algorithm/find_or_get_first.h>
#include <piejam/algorithm/for_each_visit.h>
#include <piejam/algorithm/index_of.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/io_process.h>
#include <piejam/audio/io_process_config.h>
#include <piejam/audio/multichannel_buffer.h>
#include <piejam/audio/sound_card_descriptor.h>
#include <piejam/audio/sound_card_hw_params.h>
#include <piejam/audio/sound_card_manager.h>
#include <piejam/functional/operators.h>
#include <piejam/ladspa/plugin.h>
#include <piejam/ladspa/plugin_descriptor.h>
#include <piejam/ladspa/port_descriptor.h>
#include <piejam/ladspa/processor_factory.h>
#include <piejam/midi/event.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/set_if.h>
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

struct update_devices final
    : ui::cloneable_action<update_devices, reducible_action>
{
    piejam::audio::io_sound_cards io_sound_cards;

    selected_sound_card input;
    selected_sound_card output;

    audio::sample_rate sample_rate{};
    audio::period_size period_size{};
    audio::period_count period_count{};

    void reduce(state& st) const override
    {
        st.io_sound_cards = io_sound_cards;
        st.selected_io_sound_card.in = input;
        st.selected_io_sound_card.out = output;
        st.sample_rate = sample_rate;
        st.period_size = period_size;
        st.period_count = period_count;

        auto devices = st.external_audio_state.devices.lock();

        auto update_channels = [&](auto num_channels, auto const& ids) {
            auto const ge_num_channels = greater_equal(num_channels);
            for (auto id : ids)
            {
                auto& device = devices[id];
                set_if(device.channels.left, ge_num_channels, npos);
                set_if(device.channels.right, ge_num_channels, npos);
            }
        };

        update_channels(
                input.hw_params->num_channels,
                *st.external_audio_state.inputs);

        update_channels(
                output.hw_params->num_channels,
                *st.external_audio_state.outputs);
    }
};

struct update_info final : ui::cloneable_action<update_info, reducible_action>
{
    std::size_t xruns{};
    float cpu_load{};

    void reduce(state& st) const override
    {
        st.xruns = xruns;
        st.cpu_load = cpu_load;
    }
};

} // namespace

audio_engine_middleware::audio_engine_middleware(
        thread::configuration const& audio_thread_config,
        std::span<thread::configuration const> const wt_configs,
        audio::sound_card_manager& sound_card_manager,
        ladspa::processor_factory& ladspa_processor_factory,
        std::unique_ptr<midi_input_controller> midi_controller)
    : m_audio_thread_config(audio_thread_config)
    , m_workers(wt_configs.begin(), wt_configs.end())
    , m_sound_card_manager(sound_card_manager)
    , m_ladspa_processor_factory(ladspa_processor_factory)
    , m_midi_controller(
              midi_controller ? std::move(midi_controller)
                              : make_dummy_midi_input_controller())
    , m_io_process(audio::make_dummy_io_process())
{
}

audio_engine_middleware::~audio_engine_middleware() = default;

template <class Action>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        Action const& a)
{
    mw_fs.next(a);
}

static auto
make_update_devices_action(
        audio::sound_card_manager& device_manager,
        audio::io_sound_cards const& new_devices,
        audio::io_sound_cards const& current_devices,
        std::size_t const input_index,
        std::size_t const output_index,
        audio::sample_rate const sample_rate,
        audio::period_size const period_size,
        audio::period_count const period_count) -> update_devices
{
    update_devices next_action;
    next_action.io_sound_cards = new_devices;

    auto next_device = [&](auto const& new_devices,
                           auto const& current_devices,
                           auto const current_index) {
        auto const found_index =
                current_index != npos ? algorithm::index_of(
                                                new_devices,
                                                current_devices[current_index])
                                      : npos;
        return selected_sound_card{
                .index = found_index,
                .hw_params = box(
                        found_index != npos ? device_manager.hw_params(
                                                      new_devices[found_index],
                                                      nullptr,
                                                      nullptr)
                                            : audio::sound_card_hw_params{}),
        };
    };

    next_action.input = next_device(
            new_devices.in.get(),
            current_devices.in.get(),
            input_index);

    next_action.output = next_device(
            new_devices.out.get(),
            current_devices.out.get(),
            output_index);

    auto next_value =
            [](box<audio::sound_card_hw_params> const& input_hw_params,
               box<audio::sound_card_hw_params> const& output_hw_params,
               auto&& sel,
               auto current) {
                auto const values = std::invoke(
                        std::forward<decltype(sel)>(sel),
                        input_hw_params,
                        output_hw_params);
                auto const it = algorithm::find_or_get_first(values, current);
                return it != values.end() ? *it : decltype(*it){};
            };

    next_action.sample_rate = next_value(
            next_action.input.hw_params,
            next_action.output.hw_params,
            &sample_rates,
            sample_rate);

    if (next_action.sample_rate.valid())
    {
        next_action.input.hw_params = device_manager.hw_params(
                new_devices.in.get()[next_action.input.index],
                &next_action.sample_rate,
                nullptr);
        next_action.output.hw_params = device_manager.hw_params(
                new_devices.out.get()[next_action.output.index],
                &next_action.sample_rate,
                nullptr);
    }

    next_action.period_size = next_value(
            next_action.input.hw_params,
            next_action.output.hw_params,
            &period_sizes,
            period_size);

    if (next_action.period_size.valid())
    {
        next_action.input.hw_params = device_manager.hw_params(
                new_devices.in.get()[next_action.input.index],
                &next_action.sample_rate,
                &next_action.period_size);
        next_action.output.hw_params = device_manager.hw_params(
                new_devices.out.get()[next_action.output.index],
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
        middleware_functors const& mw_fs,
        actions::apply_app_config const& a)
{
    state const& current_state = mw_fs.get_state();

    mw_fs.next(make_update_devices_action(
            m_sound_card_manager,
            current_state.io_sound_cards,
            current_state.io_sound_cards,
            algorithm::index_of(
                    current_state.io_sound_cards.in.get(),
                    a.conf.input_sound_card,
                    &audio::sound_card_descriptor::name),
            algorithm::index_of(
                    current_state.io_sound_cards.out.get(),
                    a.conf.output_sound_card,
                    &audio::sound_card_descriptor::name),
            a.conf.sample_rate,
            a.conf.period_size,
            a.conf.period_count));

    mw_fs.next(a);
}

template <>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        actions::refresh_sound_cards const&)
{
    state const& current_state = mw_fs.get_state();

    mw_fs.next(make_update_devices_action(
            m_sound_card_manager,
            m_sound_card_manager.io_descriptors(),
            current_state.io_sound_cards,
            current_state.selected_io_sound_card.in.index,
            current_state.selected_io_sound_card.out.index,
            current_state.sample_rate,
            current_state.period_size,
            current_state.period_count));
}

template <>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        actions::initiate_sound_card_selection const& action)
{
    state const& current_state = mw_fs.get_state();

    mw_fs.next(make_update_devices_action(
            m_sound_card_manager,
            current_state.io_sound_cards,
            current_state.io_sound_cards,
            action.io_dir == io_direction::input
                    ? action.index
                    : current_state.selected_io_sound_card.in.index,
            action.io_dir == io_direction::output
                    ? action.index
                    : current_state.selected_io_sound_card.out.index,
            current_state.sample_rate,
            current_state.period_size,
            current_state.period_count));
}

template <>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        actions::select_sample_rate const& action)
{
    state const& current_state = mw_fs.get_state();

    auto const srs = sample_rates_from_state(current_state);
    if (action.index < srs.size())
    {
        mw_fs.next(make_update_devices_action(
                m_sound_card_manager,
                current_state.io_sound_cards,
                current_state.io_sound_cards,
                current_state.selected_io_sound_card.in.index,
                current_state.selected_io_sound_card.out.index,
                srs[action.index],
                current_state.period_size,
                current_state.period_count));
    }
}

template <>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        actions::select_period_size const& action)
{
    state const& current_state = mw_fs.get_state();

    auto const pss = period_sizes_from_state(current_state);
    if (action.index < pss.size())
    {
        mw_fs.next(make_update_devices_action(
                m_sound_card_manager,
                current_state.io_sound_cards,
                current_state.io_sound_cards,
                current_state.selected_io_sound_card.in.index,
                current_state.selected_io_sound_card.out.index,
                current_state.sample_rate,
                pss[action.index],
                current_state.period_count));
    }
}

template <>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        actions::select_period_count const& action)
{
    state const& current_state = mw_fs.get_state();

    auto const pcs = period_counts_from_state(current_state);
    if (action.index < pcs.size())
    {
        mw_fs.next(make_update_devices_action(
                m_sound_card_manager,
                current_state.io_sound_cards,
                current_state.io_sound_cards,
                current_state.selected_io_sound_card.in.index,
                current_state.selected_io_sound_card.out.index,
                current_state.sample_rate,
                current_state.period_size,
                pcs[action.index]));
    }
}

template <>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        actions::activate_midi_device const& action)
{
    if (m_midi_controller->activate_input_device(action.device_id))
    {
        mw_fs.next(action);
    }
}

template <>
void
audio_engine_middleware::process_device_action(
        middleware_functors const& mw_fs,
        actions::deactivate_midi_device const& action)
{
    m_midi_controller->deactivate_input_device(action.device_id);
    mw_fs.next(action);
}

template <class Action>
void
audio_engine_middleware::process_engine_action(
        middleware_functors const& mw_fs,
        Action const& a)
{
    mw_fs.next(a);
    rebuild(mw_fs.get_state());
}

template <class Parameter>
void
audio_engine_middleware::process_engine_action(
        middleware_functors const& mw_fs,
        actions::set_parameter_value<Parameter> const& a)
{
    if (m_engine)
    {
        m_engine->set_parameter_value(a.id, a.value);
    }

    mw_fs.next(a);
}

template <std::ranges::range Parameters>
static void
collect_parameter_updates(
        Parameters&& params,
        audio_engine const& engine,
        actions::audio_engine_sync_update& action)
{
    algorithm::for_each_visit(std::forward<Parameters>(params), [&](auto id) {
        if (auto value = engine.get_parameter_update(id); value)
        {
            action.push_back(id, *value);
        }
    });
}

template <std::ranges::range Streams>
static void
collect_stream_updates(
        Streams const& streams,
        audio_engine const& engine,
        actions::audio_engine_sync_update& action)
{
    for (auto id : streams)
    {
        if (auto captured = engine.get_stream(id); !captured->empty())
        {
            action.streams.emplace(id, std::move(captured));
        }
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        middleware_functors const& mw_fs,
        actions::request_audio_engine_sync const&)
{
    if (m_engine)
    {
        state const& st = mw_fs.get_state();

        actions::audio_engine_sync_update next_action;

        collect_parameter_updates(
                st.midi_assignments.get() | std::views::keys,
                *m_engine,
                next_action);

        collect_stream_updates(
                st.streams | std::views::keys,
                *m_engine,
                next_action);

        if (!next_action.empty())
        {
            mw_fs.next(next_action);
        }
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        middleware_functors const& mw_fs,
        actions::stop_recording const& a)
{
    mw_fs.next(a);
    rebuild(mw_fs.get_state());
}

template <>
void
audio_engine_middleware::process_engine_action(
        middleware_functors const& mw_fs,
        actions::request_info_update const&)
{
    {
        update_info next_action;
        next_action.xruns = m_io_process->xruns();
        next_action.cpu_load = m_io_process->cpu_load();

        mw_fs.next(next_action);
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
                        *mw_fs.get_state().midi_learning,
                        midi_assignment{
                                .channel = cc_event->channel,
                                .control_type = midi_assignment::type::cc,
                                .control_id = cc_event->data.cc});

                mw_fs.next(next_action);
            }

            mw_fs.next(actions::stop_midi_learning{});

            rebuild(mw_fs.get_state());
        }
    }
}

void
audio_engine_middleware::close_sound_card()
{
    m_io_process = audio::make_dummy_io_process();

    // The engine is executed by a device, we can safely destroy it after device
    // was closed.
    m_engine.reset();
}

void
audio_engine_middleware::open_sound_card(state const& st)
{
    if (st.selected_io_sound_card.in.index == npos ||
        st.selected_io_sound_card.out.index == npos ||
        st.sample_rate.invalid() || st.period_size.invalid() ||
        st.period_count.invalid())
    {
        return;
    }

    try
    {
        auto io_process = m_sound_card_manager.make_io_process(
                st.io_sound_cards.in.get()[st.selected_io_sound_card.in.index],
                st.io_sound_cards.out
                        .get()[st.selected_io_sound_card.out.index],
                audio::io_process_config{
                        audio::sound_card_config{
                                st.selected_io_sound_card.in.hw_params
                                        ->interleaved,
                                st.selected_io_sound_card.in.hw_params->format,
                                st.selected_io_sound_card.in.hw_params
                                        ->num_channels},
                        audio::sound_card_config{
                                st.selected_io_sound_card.out.hw_params
                                        ->interleaved,
                                st.selected_io_sound_card.out.hw_params->format,
                                st.selected_io_sound_card.out.hw_params
                                        ->num_channels},
                        audio::sound_card_buffer_config{
                                st.sample_rate,
                                st.period_size,
                                st.period_count}});
        m_io_process.swap(io_process);
    }
    catch (std::exception const& err)
    {
        auto const* const message = err.what();
        spdlog::error("opening device failed: {}", message);
    }
}

void
audio_engine_middleware::start_engine(state const& st)
{
    BOOST_ASSERT(m_io_process);

    if (m_io_process->is_open())
    {
        m_engine = std::make_unique<audio_engine>(
                m_workers,
                st.sample_rate,
                st.selected_io_sound_card.in.hw_params->num_channels,
                st.selected_io_sound_card.out.hw_params->num_channels);

        m_io_process->start(
                m_audio_thread_config,
                [engine = m_engine.get()](auto const& in, auto const& out) {
                    engine->init_process(in, out);
                },
                [engine = m_engine.get()](auto const buffer_size) {
                    engine->process(buffer_size);
                });

        rebuild(st);
    }
}

void
audio_engine_middleware::rebuild(state const& st)
{
    if (!m_engine || !m_io_process->is_running())
    {
        return;
    }

    if (!m_engine->rebuild(
                st,
                [this, sr = st.sample_rate](ladspa::instance_id id) {
                    return m_ladspa_processor_factory.make_processor(id, sr);
                },
                m_midi_controller->make_input_event_handler()))
    {
        spdlog::error("Rebuilding audio engine graph failed.");
    }
}

void
audio_engine_middleware::operator()(
        middleware_functors const& mw_fs,
        action const& action)
{
    if (auto a = dynamic_cast<actions::audio_io_process_action const*>(&action))
    {
        auto const& st = mw_fs.get_state();

        if (st.recording)
        {
            process_engine_action(mw_fs, actions::stop_recording{});
        }

        close_sound_card();

        auto v = ui::make_action_visitor<
                actions::audio_io_process_action_visitor>(
                [&](auto&& a) { process_device_action(mw_fs, a); });

        a->visit(v);

        open_sound_card(st);
        start_engine(st);
    }
    else if (
            auto a = dynamic_cast<actions::audio_engine_action const*>(&action))
    {
        auto v = ui::make_action_visitor<actions::audio_engine_action_visitor>(
                [&](auto&& a) { process_engine_action(mw_fs, a); });

        a->visit(v);
    }
    else
    {
        mw_fs.next(action);
    }
}

} // namespace piejam::runtime
