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
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/delete_bus.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/device_action_visitor.h>
#include <piejam/runtime/actions/engine_action_visitor.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/move_fx_module.h>
#include <piejam/runtime/actions/request_levels_update.h>
#include <piejam/runtime/actions/select_bus_channel.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_samplerate.h>
#include <piejam/runtime/actions/set_bus_solo.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/audio_engine.h>
#include <piejam/runtime/fx/ladspa_manager.h>
#include <piejam/runtime/state.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

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

    audio::samplerate_t samplerate{};
    audio::period_size_t period_size{};

    auto reduce(state const& st) const -> state override
    {
        auto new_st = st;

        new_st.pcm_devices = pcm_devices;
        new_st.input = input;
        new_st.output = output;
        new_st.samplerate = samplerate;
        new_st.period_size = period_size;

        auto const& in_ids = *st.mixer_state.inputs;
        auto const& out_ids = *st.mixer_state.outputs;

        std::size_t const num_in_channels = input.hw_params->num_channels;
        for (auto const& in_id : in_ids)
        {
            new_st.mixer_state.buses.update(
                    in_id,
                    [num_in_channels](mixer::bus& bus) {
                        update_channel(
                                bus.device_channels.left,
                                num_in_channels);
                        update_channel(
                                bus.device_channels.right,
                                num_in_channels);
                    });
        }

        std::size_t const num_out_channels = output.hw_params->num_channels;
        for (auto const& out_id : out_ids)
        {
            new_st.mixer_state.buses.update(
                    out_id,
                    [num_out_channels](mixer::bus& bus) {
                        update_channel(
                                bus.device_channels.left,
                                num_out_channels);
                        update_channel(
                                bus.device_channels.right,
                                num_out_channels);
                    });
        }

        return new_st;
    }
};

template <io_direction D>
struct select_device final : ui::cloneable_action<select_device<D>, action>
{
    selected_device device;
    unsigned samplerate{};
    unsigned period_size{};

    auto reduce(state const&) const -> state override;
};

template <>
auto
select_device<io_direction::input>::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.input = device;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    clear_mixer_buses<io_direction::input>(new_st);

    std::size_t const num_channels = device.hw_params->num_channels;
    for (std::size_t index = 0; index < num_channels; ++index)
    {
        add_mixer_bus<io_direction::input>(
                new_st,
                fmt::format("In {}", index + 1),
                audio::bus_type::mono,
                channel_index_pair(index));
    }

    return new_st;
}

template <>
auto
select_device<io_direction::output>::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.output = device;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    clear_mixer_buses<io_direction::output>(new_st);

    if (auto const num_channels = device.hw_params->num_channels)
    {
        add_mixer_bus<io_direction::output>(
                new_st,
                "Main",
                audio::bus_type::stereo,
                channel_index_pair(
                        num_channels > 0 ? 0 : npos,
                        num_channels > 1 ? 1 : npos));
    }

    return new_st;
}

using select_input_device = select_device<io_direction::input>;
using select_output_device = select_device<io_direction::output>;

struct update_levels final : ui::cloneable_action<update_levels, action>
{
    std::vector<std::pair<stereo_level_parameter_id, stereo_level>> levels;

    auto reduce(state const& st) const -> state override
    {
        auto new_st = st;

        for (auto&& [id, lvl] : levels)
            new_st.params.set(id, lvl);

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

struct insert_ladspa_fx_module_action final
    : ui::cloneable_action<insert_ladspa_fx_module_action, action>
{
    mixer::bus_id fx_chain_bus;
    std::size_t position{};
    fx::ladspa_instance_id instance_id;
    audio::ladspa::plugin_descriptor plugin_desc;
    std::span<audio::ladspa::port_descriptor const> control_inputs;
    std::vector<fx::parameter_assignment> initial_assignments;

    auto reduce(state const& st) const -> state override
    {
        auto new_st = st;

        runtime::insert_ladspa_fx_module(
                new_st,
                fx_chain_bus,
                position,
                instance_id,
                plugin_desc,
                control_inputs,
                initial_assignments);

        return new_st;
    }
};

struct insert_missing_ladspa_fx_module_action final
    : ui::cloneable_action<insert_missing_ladspa_fx_module_action, action>
{
    mixer::bus_id fx_chain_bus;
    std::size_t position{};
    fx::unavailable_ladspa unavailable_ladspa;
    std::string name;

    auto reduce(state const& st) const -> state override
    {
        auto new_st = st;

        runtime::insert_missing_ladspa_fx_module(
                new_st,
                fx_chain_bus,
                position,
                unavailable_ladspa,
                name);

        return new_st;
    }
};

} // namespace

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
    if (auto a = dynamic_cast<actions::device_action const*>(&action))
    {
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
        m_next(action);
    }
}

template <class Action>
void
audio_engine_middleware::process_device_action(Action const& a)
{
    m_next(a);
}

static auto
next_samplerate_and_period_size(
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params,
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

template <>
void
audio_engine_middleware::process_device_action(
        actions::apply_app_config const& a)
{
    state const& current_state = m_get_state();

    update_devices prep_action;
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

template <>
void
audio_engine_middleware::process_device_action(actions::refresh_devices const&)
{
    state const& current_state = m_get_state();

    update_devices next_action;
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

template <>
void
audio_engine_middleware::process_device_action(
        actions::initiate_device_selection const& action)
{
    state const& current_state = m_get_state();

    std::size_t const index = action.index;

    if (action.input)
    {
        select_input_device next_action;
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
        select_output_device next_action;
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

template <class Action>
void
audio_engine_middleware::process_engine_action(Action const& a)
{
    m_next(a);
    rebuild();
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::delete_fx_module const& a)
{
    auto const& st = m_get_state();

    if (fx::module const* const fx_mod = st.fx_modules[a.fx_mod_id])
    {
        auto const fx_instance_id = fx_mod->fx_instance_id;

        m_next(a);

        rebuild();

        if (fx::ladspa_instance_id const* const instance_id =
                    std::get_if<fx::ladspa_instance_id>(&fx_instance_id))
        {
            m_ladspa_fx_manager->unload(*instance_id);
        }
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::load_ladspa_fx_plugin const& a)
{
    auto const& st = m_get_state();

    if (auto plugin_desc =
                find_ladspa_plugin_descriptor(st.fx_registry, a.plugin_id))
    {
        if (auto id = m_ladspa_fx_manager->load(*plugin_desc))
        {
            insert_ladspa_fx_module_action next_action;
            next_action.fx_chain_bus = a.fx_chain_bus;
            next_action.position = a.position;
            next_action.instance_id = id;
            next_action.plugin_desc = *plugin_desc;
            next_action.control_inputs =
                    m_ladspa_fx_manager->control_inputs(id);
            next_action.initial_assignments = a.initial_assignments;

            m_next(next_action);

            rebuild();

            return;
        }
        else
        {
            spdlog::error("failed to load ladspa fx plugin: {}", a.name);
        }
    }

    insert_missing_ladspa_fx_module_action next_action;
    next_action.fx_chain_bus = a.fx_chain_bus;
    next_action.position = a.position;
    next_action.unavailable_ladspa = {a.plugin_id, a.initial_assignments};
    next_action.name = a.name;
    m_next(next_action);
}

template <class Parameter>
void
audio_engine_middleware::process_engine_action(
        actions::set_parameter_value<Parameter> const& a)
{
    m_next(a);

    if (m_engine)
    {
        m_engine->set_parameter(a.id, *m_get_state().params.get(a.id));
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::set_input_bus_solo const& a)
{
    m_next(a);

    if (m_engine)
    {
        m_engine->set_input_solo(m_get_state().mixer_state.input_solo_id);
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::request_levels_update const& a)
{
    if (m_engine)
    {
        update_levels next_action;

        for (auto&& id : a.level_ids)
        {
            if (auto lvl = m_engine->get_level(id))
                next_action.levels.emplace_back(id, *lvl);
        }

        m_next(next_action);
    }
}

template <>
void
audio_engine_middleware::process_engine_action(
        actions::request_info_update const&)
{
    update_info next_action;
    next_action.xruns = m_device->xruns();
    next_action.cpu_load = m_device->cpu_load();

    m_next(next_action);
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
    if (!m_engine || !m_device->is_running())
        return;

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
