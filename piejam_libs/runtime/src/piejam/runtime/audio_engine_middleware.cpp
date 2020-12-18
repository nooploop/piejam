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
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/ladspa/plugin.h>
#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/runtime/actions/add_bus.h>
#include <piejam/runtime/actions/add_ladspa_fx_module.h>
#include <piejam/runtime/actions/add_missing_ladspa_fx_module.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/delete_bus.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/device_action_visitor.h>
#include <piejam/runtime/actions/engine_action_visitor.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/load_ladspa_fx_plugin.h>
#include <piejam/runtime/actions/request_levels_update.h>
#include <piejam/runtime/actions/select_bus_channel.h>
#include <piejam/runtime/actions/select_device.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_samplerate.h>
#include <piejam/runtime/actions/set_bus_mute.h>
#include <piejam/runtime/actions/set_bus_pan_balance.h>
#include <piejam/runtime/actions/set_bus_solo.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/actions/update_devices.h>
#include <piejam/runtime/actions/update_info.h>
#include <piejam/runtime/actions/update_levels.h>
#include <piejam/runtime/audio_engine.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/fx/ladspa_manager.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

namespace piejam::runtime
{

audio_engine_middleware::audio_engine_middleware(
        thread::configuration const& audio_thread_config,
        std::span<thread::configuration const> const& wt_configs,
        get_pcm_io_descriptors_f get_pcm_io_descriptors,
        get_hw_params_f get_hw_params,
        device_factory_f device_factory,
        get_state_f get_state,
        next_f next)
    : m_audio_thread_config(audio_thread_config)
    , m_wt_configs(wt_configs.begin(), wt_configs.end())
    , m_get_pcm_io_descriptors(std::move(get_pcm_io_descriptors))
    , m_get_hw_params(std::move(get_hw_params))
    , m_device_factory(std::move(device_factory))
    , m_get_state(std::move(get_state))
    , m_next(std::move(next))
    , m_ladspa_fx_manager(std::make_unique<fx::ladspa_manager>())
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
            [](actions::select_input_device const&) {
                BOOST_ASSERT_MSG(false, "should not be sent by user");
            },
            [](actions::select_output_device const&) {
                BOOST_ASSERT_MSG(false, "should not be sent by user");
            },
            [this](actions::select_samplerate const& a) { m_next(a); },
            [this](actions::select_period_size const& a) { m_next(a); });

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
    state const& current_state = m_get_state();

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
    state const& current_state = m_get_state();

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
    state const& current_state = m_get_state();

    std::size_t const index = action.index;

    if (action.input)
    {
        actions::select_input_device next_action;
        auto const& descriptors = current_state.pcm_devices->inputs;
        next_action.device = {index, m_get_hw_params(descriptors[index])};

        std::tie(next_action.samplerate, next_action.period_size) =
                next_samplerate_and_period_size(
                        next_action.device.hw_params,
                        current_state.output.hw_params,
                        current_state.samplerate,
                        current_state.period_size);

        m_next(next_action);
    }
    else
    {
        actions::select_output_device next_action;
        auto const& descriptors = current_state.pcm_devices->outputs;
        next_action.device = {index, m_get_hw_params(descriptors[index])};

        std::tie(next_action.samplerate, next_action.period_size) =
                next_samplerate_and_period_size(
                        current_state.input.hw_params,
                        next_action.device.hw_params,
                        current_state.samplerate,
                        current_state.period_size);

        m_next(next_action);
    }
}

static auto
find_ladspa_plugin_descriptor(
        fx::registry const& registry,
        audio::ladspa::plugin_id_t id)
        -> audio::ladspa::plugin_descriptor const*
{
    for (auto const& item : *registry.entries)
    {
        if (auto pd = std::get_if<audio::ladspa::plugin_descriptor>(&item);
            pd && pd->id == id)
            return pd;
    }

    return nullptr;
}

void
audio_engine_middleware::process_engine_action(
        actions::engine_action const& action)
{
    auto v = ui::make_action_visitor<actions::engine_action_visitor>(
            [this](actions::select_bus_channel const& a) {
                m_next(a);

                if (m_engine)
                    rebuild();
            },
            [this](actions::add_bus const& a) {
                m_next(a);

                if (m_engine)
                    rebuild();
            },
            [this](actions::delete_bus const& a) {
                m_next(a);

                if (m_engine)
                    rebuild();
            },
            [this](actions::insert_internal_fx_module const& a) {
                m_next(a);

                if (m_engine)
                    rebuild();
            },
            [this](actions::delete_fx_module const& a) {
                auto const& st = m_get_state();

                if (fx::module const* const fx_mod =
                            st.fx_modules.get()[a.fx_mod_id])
                {
                    m_next(a);

                    if (m_engine)
                        rebuild();

                    if (fx::ladspa_instance_id const* const instance_id =
                                std::get_if<fx::ladspa_instance_id>(
                                        &fx_mod->fx_instance_id))
                    {
                        m_ladspa_fx_manager->unload(*instance_id);
                    }
                }
            },
            [this](actions::load_ladspa_fx_plugin const& a) {
                auto const& st = m_get_state();

                if (auto plugin_desc = find_ladspa_plugin_descriptor(
                            st.fx_registry,
                            a.plugin_id))
                {
                    if (auto id = m_ladspa_fx_manager->load(*plugin_desc))
                    {
                        actions::add_ladspa_fx_module next_action;
                        next_action.fx_chain_bus = a.fx_chain_bus;
                        next_action.instance_id = id;
                        next_action.plugin_desc = *plugin_desc;
                        next_action.control_inputs =
                                m_ladspa_fx_manager->control_inputs(id);

                        m_next(next_action);

                        if (m_engine)
                            rebuild();
                    }
                    else
                    {
                        spdlog::error(
                                "failed to load ladspa fx plugin: {}",
                                a.name);

                        actions::add_missing_ladspa_fx_module next_action;
                        next_action.fx_chain_bus = a.fx_chain_bus;
                        next_action.missing_id = {a.plugin_id};
                        next_action.name = a.name;
                        m_next(next_action);
                    }
                }
                else
                {
                    actions::add_missing_ladspa_fx_module next_action;
                    next_action.fx_chain_bus = a.fx_chain_bus;
                    next_action.missing_id = {a.plugin_id};
                    next_action.name = a.name;
                    m_next(next_action);
                }
            },
            [this](actions::set_bool_parameter const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_bool_parameter(
                            a.id,
                            *m_get_state().params.get(a.id));
                }
            },
            [this](actions::set_float_parameter const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_float_parameter(
                            a.id,
                            *m_get_state().params.get(a.id));
                }
            },
            [this](actions::set_int_parameter const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_int_parameter(
                            a.id,
                            *m_get_state().params.get(a.id));
                }
            },
            [this](actions::set_input_bus_solo const& a) {
                m_next(a);

                if (m_engine)
                {
                    m_engine->set_input_solo(
                            m_get_state().mixer_state.input_solo_id);
                }
            },
            [this](actions::request_levels_update const& a) {
                if (m_engine)
                {
                    actions::update_levels next_action;

                    for (auto&& id : a.level_ids)
                    {
                        if (auto lvl = m_engine->get_level(id))
                            next_action.levels.emplace_back(id, *lvl);
                    }

                    m_next(next_action);
                }
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
                m_wt_configs,
                state.samplerate,
                state.input.hw_params->num_channels,
                state.output.hw_params->num_channels);

        m_device->start(
                m_audio_thread_config,
                [engine = m_engine.get()](auto const& in, auto const& out) {
                    engine->operator()(in, out);
                });

        rebuild();
    }
}

void
audio_engine_middleware::rebuild()
{
    BOOST_ASSERT(m_engine);
    auto const& st = m_get_state();
    m_engine->rebuild(
            st.mixer_state,
            st.fx_modules,
            st.fx_parameters,
            st.params,
            [this, sr = st.samplerate](fx::ladspa_instance_id id) {
                return m_ladspa_fx_manager->make_processor(id, sr);
            });
}

} // namespace piejam::runtime
