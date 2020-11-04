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

#include <piejam/runtime/audio_engine_middleware.h>

#include <piejam/algorithm/find_or_get_first.h>
#include <piejam/algorithm/index_of.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/device_action_visitor.h>
#include <piejam/runtime/actions/device_actions.h>
#include <piejam/runtime/actions/engine_action_visitor.h>
#include <piejam/runtime/actions/engine_actions.h>
#include <piejam/runtime/audio_engine.h>
#include <piejam/runtime/audio_state.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

namespace piejam::runtime
{

audio_engine_middleware::audio_engine_middleware(
        thread::configuration const& audio_thread_config,
        get_pcm_io_descriptors_f get_pcm_io_descriptors,
        get_hw_params_f get_hw_params,
        device_factory_f device_factory,
        get_state_f get_state,
        next_f next)
    : m_audio_thread_config(audio_thread_config)
    , m_get_pcm_io_descriptors(std::move(get_pcm_io_descriptors))
    , m_get_hw_params(std::move(get_hw_params))
    , m_device_factory(std::move(device_factory))
    , m_get_state(std::move(get_state))
    , m_next(std::move(next))
{
    BOOST_ASSERT(m_get_hw_params);
    BOOST_ASSERT(m_device_factory);
    BOOST_ASSERT(m_get_state);
    BOOST_ASSERT(m_next);
}

audio_engine_middleware::~audio_engine_middleware() = default;

void
audio_engine_middleware::operator()(action const& action)
{
    if (auto const* const a =
                dynamic_cast<actions::device_action const*>(&action))
    {
        process_device_action(*a);
    }
    else if (
            auto const* const a =
                    dynamic_cast<actions::engine_action const*>(&action))
    {
        process_engine_action(*a);
    }
    else
    {
        m_next(action);
    }
}

void
audio_engine_middleware::process_device_action(
        actions::device_action const& action)
{
    close_device();

    auto v = ui::make_action_visitor<actions::device_action_visitor>(
            [this](actions::apply_app_config const& a) {
                process_apply_app_config_action(a);
            },
            [this](actions::refresh_devices const& a) {
                process_refresh_devices_action(a);
            },
            [](actions::update_devices const&) {
                BOOST_ASSERT_MSG(false, "should not be sent by user");
            },
            [this](actions::initiate_device_selection const& a) {
                process_initiate_device_selection_action(a);
            },
            [](actions::select_device const&) {
                BOOST_ASSERT_MSG(false, "should not be sent by user");
            },
            [this](actions::select_samplerate const& a) { m_next(a); },
            [this](actions::select_period_size const& a) { m_next(a); },
            [this](actions::select_bus_channel const& a) { m_next(a); },
            [this](actions::add_device_bus const& a) { m_next(a); },
            [this](actions::delete_device_bus const& a) { m_next(a); });

    action.visit(v);

    open_device();
    start_engine();
}

static auto
next_samplerate_and_period_size(
        container::box<audio::pcm_hw_params> input_hw_params,
        container::box<audio::pcm_hw_params> output_hw_params,
        audio::samplerate_t const current_samplerate,
        audio::period_size_t const current_period_size)
        -> std::pair<audio::samplerate_t, audio::period_size_t>
{
    auto next_value = [&](auto&& f, auto&& current) {
        auto const values = f(input_hw_params, output_hw_params);
        BOOST_ASSERT(!values.empty());
        return *algorithm::find_or_get_first(values, current);
    };

    return {next_value(&samplerates, current_samplerate),
            next_value(&period_sizes, current_period_size)};
}

void
audio_engine_middleware::process_apply_app_config_action(
        actions::apply_app_config const& a)
{
    audio_state const& current_state = m_get_state();

    actions::update_devices prep_action;
    prep_action.pcm_devices = current_state.pcm_devices;

    if (auto index = algorithm::index_of_if(
                current_state.pcm_devices->inputs,
                [&](auto const& d) {
                    return d.name == a.conf.input_device_name;
                });
        index != npos)
    {
        prep_action.input = {
                index,
                m_get_hw_params(prep_action.pcm_devices->inputs[index])};
    }
    else
    {
        prep_action.input = current_state.input;
    }

    if (auto index = algorithm::index_of_if(
                current_state.pcm_devices->outputs,
                [&](auto const& d) {
                    return d.name == a.conf.output_device_name;
                });
        index != npos)
    {
        prep_action.output = {
                index,
                m_get_hw_params(prep_action.pcm_devices->outputs[index])};
    }
    else
    {
        prep_action.output = current_state.output;
    }

    std::tie(prep_action.samplerate, prep_action.period_size) =
            next_samplerate_and_period_size(
                    prep_action.input.hw_params,
                    prep_action.output.hw_params,
                    a.conf.samplerate,
                    a.conf.period_size);

    m_next(prep_action);

    m_next(a);
}

void
audio_engine_middleware::process_refresh_devices_action(
        actions::refresh_devices const&)
{
    audio_state const& current_state = m_get_state();

    actions::update_devices next_action;
    next_action.pcm_devices = m_get_pcm_io_descriptors();

    auto next_device = [this](auto const& new_devices,
                              auto const& current_devices,
                              auto const current_index) -> selected_device {
        auto const found_index = algorithm::index_of(
                new_devices,
                current_devices[current_index]);
        auto const next_index = found_index == npos ? 0 : found_index;
        return {next_index, m_get_hw_params(new_devices[next_index])};
    };

    next_action.input = next_device(
            next_action.pcm_devices->inputs,
            current_state.pcm_devices->inputs,
            current_state.input.index);

    next_action.output = next_device(
            next_action.pcm_devices->outputs,
            current_state.pcm_devices->outputs,
            current_state.output.index);

    std::tie(next_action.samplerate, next_action.period_size) =
            next_samplerate_and_period_size(
                    next_action.input.hw_params,
                    next_action.output.hw_params,
                    current_state.samplerate,
                    current_state.period_size);

    m_next(next_action);
}

void
audio_engine_middleware::process_initiate_device_selection_action(
        actions::initiate_device_selection const& action)
{
    audio_state const& current_state = m_get_state();

    std::size_t const index = action.index;

    actions::select_device next_action;
    next_action.input = action.input;
    auto const& descriptors = action.input ? current_state.pcm_devices->inputs
                                           : current_state.pcm_devices->outputs;
    BOOST_ASSERT(index < descriptors.size());
    next_action.device = {index, m_get_hw_params(descriptors[index])};

    if (action.input)
    {
        std::tie(next_action.samplerate, next_action.period_size) =
                next_samplerate_and_period_size(
                        next_action.device.hw_params,
                        current_state.output.hw_params,
                        current_state.samplerate,
                        current_state.period_size);
    }
    else
    {
        std::tie(next_action.samplerate, next_action.period_size) =
                next_samplerate_and_period_size(
                        current_state.input.hw_params,
                        next_action.device.hw_params,
                        current_state.samplerate,
                        current_state.period_size);
    }

    m_next(next_action);
}

void
audio_engine_middleware::process_engine_action(
        actions::engine_action const& action)
{
    auto v = ui::make_action_visitor<actions::engine_action_visitor>(
            [this](actions::set_input_channel_volume const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_input_channel_volume(a.index, a.volume);
                }
            },
            [this](actions::set_input_channel_pan const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_input_channel_pan_balance(a.index, a.pan);
                }
            },
            [this](actions::set_output_channel_volume const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_output_channel_volume(a.index, a.volume);
                }
            },
            [this](actions::set_output_channel_balance const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_output_channel_balance(a.index, a.balance);
                }
            },
            [this](actions::request_levels_update const&) {
                audio_state const& current_state = m_get_state();

                std::size_t const num_inputs =
                        current_state.mixer_state.inputs.size();
                std::size_t const num_outputs =
                        current_state.mixer_state.outputs.size();

                actions::update_levels next_action;
                next_action.in_levels.reserve(num_inputs);
                next_action.out_levels.reserve(num_outputs);

                if (m_engine)
                {
                    for (std::size_t index = 0; index < num_inputs; ++index)
                        next_action.in_levels.push_back(
                                m_engine->get_input_level(index));

                    for (std::size_t index = 0; index < num_outputs; ++index)
                        next_action.out_levels.push_back(
                                m_engine->get_output_level(index));
                }

                m_next(next_action);
            },
            [](actions::update_levels const&) {},
            [this](actions::request_info_update const&) {
                actions::update_info next_action;
                next_action.xruns = m_device->xruns();
                next_action.cpu_load = m_device->cpu_load();

                m_next(next_action);
            },
            [](actions::update_info const&) {});

    action.visit(v);
}

void
audio_engine_middleware::close_device()
{
    // We handle all the exceptions here, because stopping/closing might fail,
    // e.g. device was removed, but we still want to continue to proceed after
    // that.
    try
    {
        if (m_device->is_running())
            m_device->stop();
    }
    catch (std::exception const& err)
    {
        spdlog::error("stopping device failed: {}", err.what());
    }

    try
    {
        m_device->close();
    }
    catch (std::exception const& err)
    {
        spdlog::error("closing device failed: {}", err.what());
    }

    m_device = std::make_unique<piejam::audio::dummy_device>();

    // The engine is executed by a device, we can safely destroy it after device
    // was closed.
    m_engine.reset();
}

void
audio_engine_middleware::open_device()
{
    try
    {
        auto device = m_device_factory(m_get_state());
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
        auto const& state = m_get_state();

        m_engine = std::make_unique<audio_engine>(
                state.samplerate,
                state.input.hw_params->num_channels,
                state.output.hw_params->num_channels,
                state.mixer_state);

        m_device->start(
                m_audio_thread_config,
                [engine = m_engine.get()](auto const& in, auto const& out) {
                    engine->operator()(in, out);
                });
    }
}

} // namespace piejam::runtime
