// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/selectors.h>

#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/solo_group.h>
#include <piejam/runtime/state.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/multichannel_buffer.h>
#include <piejam/boxify_result.h>
#include <piejam/functional/memo.h>
#include <piejam/npos.h>
#include <piejam/reselect/selector.h>

#include <boost/hof/match.hpp>
#include <boost/hof/unpack.hpp>

namespace piejam::runtime::selectors
{

static auto get_sample_rate =
        memo([](unique_box<audio::sound_card_hw_params> const& input_hw_params,
                unique_box<audio::sound_card_hw_params> const& output_hw_params,
                audio::sample_rate const current) {
            return box<sample_rate_choice>{
                    std::in_place,
                    runtime::sample_rates(input_hw_params, output_hw_params),
                    current};
        });

selector<box<sample_rate_choice>> const select_sample_rate([](state const& st) {
    return get_sample_rate(
            st.selected_io_sound_card.in.hw_params,
            st.selected_io_sound_card.out.hw_params,
            st.sample_rate);
});

static auto get_period_size =
        memo([](unique_box<audio::sound_card_hw_params> const& input_hw_params,
                unique_box<audio::sound_card_hw_params> const& output_hw_params,
                audio::period_size const current) {
            return box<period_size_choice>{
                    std::in_place,
                    runtime::period_sizes(input_hw_params, output_hw_params),
                    current};
        });

selector<box<period_size_choice>> const select_period_size([](state const& st) {
    return get_period_size(
            st.selected_io_sound_card.in.hw_params,
            st.selected_io_sound_card.out.hw_params,
            st.period_size);
});

static auto get_period_count =
        memo([](unique_box<audio::sound_card_hw_params> const& input_hw_params,
                unique_box<audio::sound_card_hw_params> const& output_hw_params,
                audio::period_count const current) {
            return box<period_count_choice>{
                    std::in_place,
                    runtime::period_counts(input_hw_params, output_hw_params),
                    current};
        });

selector<box<period_count_choice>> const
        select_period_count([](state const& st) {
            return get_period_count(
                    st.selected_io_sound_card.in.hw_params,
                    st.selected_io_sound_card.out.hw_params,
                    st.period_count);
        });

selector<float> const select_buffer_latency([](state const& st) {
    return st.sample_rate.get() != 0
                   ? (st.period_size.get() * st.period_count.get() * 1000.f) /
                             st.sample_rate.as_float()
                   : 0.f;
});

static auto get_sound_card = memo(
        [](unique_box<std::vector<audio::sound_card_descriptor>> const& descs,
           std::size_t const index) {
            return box<sound_card_choice>{
                    std::in_place,
                    algorithm::transform_to_vector(
                            descs.get(),
                            &audio::sound_card_descriptor::name),
                    index};
        });

selector<box<sound_card_choice>> const
        select_input_sound_card([](state const& st) {
            return get_sound_card(
                    st.io_sound_cards.in,
                    st.selected_io_sound_card.in.index);
        });

selector<box<sound_card_choice>> const
        select_output_sound_card([](state const& st) {
            return get_sound_card(
                    st.io_sound_cards.out,
                    st.selected_io_sound_card.out.index);
        });

auto
make_num_device_channels_selector(io_direction const bd)
        -> selector<std::size_t>
{
    switch (bd)
    {
        case io_direction::input:
            return selector<std::size_t>([](state const& st) -> std::size_t {
                return st.selected_io_sound_card.in.hw_params->num_channels;
            });

        case io_direction::output:
            return selector<std::size_t>([](state const& st) -> std::size_t {
                return st.selected_io_sound_card.out.hw_params->num_channels;
            });
    }
}

auto
make_device_bus_list_selector(io_direction const bd)
        -> selector<box<device_io::bus_list_t>>
{
    switch (bd)
    {
        case io_direction::input:
            return [](state const& st) { return st.device_io_state.inputs; };

        case io_direction::output:
            return [](state const& st) { return st.device_io_state.outputs; };
    }
}

selector<boxed_vector<mixer::channel_id>> const select_mixer_input_channels(
        [](state const& st) { return st.mixer_state.inputs; });

selector<mixer::channel_id> const select_mixer_main_channel(
        [](state const& st) { return st.mixer_state.main; });

auto
make_mixer_channel_bus_type_selector(mixer::channel_id const channel_id)
        -> selector<audio::bus_type>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->bus_type : audio::bus_type::mono;
    };
}

auto
make_mixer_channel_volume_parameter_selector(mixer::channel_id const channel_id)
        -> selector<float_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->volume : float_parameter_id{};
    };
}

auto
make_mixer_channel_pan_balance_parameter_selector(
        mixer::channel_id const channel_id) -> selector<float_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->pan_balance : float_parameter_id{};
    };
}

auto
make_mixer_channel_record_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->record : bool_parameter_id{};
    };
}

auto
make_mixer_channel_mute_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->mute : bool_parameter_id{};
    };
}

auto
make_mixer_channel_solo_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->solo : bool_parameter_id{};
    };
}

auto
make_mixer_channel_peak_level_parameter_selector(
        mixer::channel_id const channel_id)
        -> selector<stereo_level_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->peak_level : stereo_level_parameter_id{};
    };
}

auto
make_mixer_channel_rms_level_parameter_selector(
        mixer::channel_id const channel_id)
        -> selector<stereo_level_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->rms_level : stereo_level_parameter_id{};
    };
}

static struct
{
    auto operator()(
            audio::bus_type const bus_type,
            device_io::buses_t const& devices,
            box<device_io::bus_list_t> const& from_device_list) const
            -> boxed_vector<mixer_device_route>
    {
        std::vector<mixer_device_route> result;
        for (auto device_bus_id : *from_device_list)
        {
            if (device_io::bus const* const device_bus =
                        devices.find(device_bus_id);
                device_bus && device_bus->bus_type == bus_type)
            {
                result.emplace_back(mixer_device_route{
                        .bus_id = device_bus_id,
                        .name = *device_bus->name});
            }
        }
        return box_(std::move(result));
    }

    auto operator()(audio::bus_type const bus_type) const
    {
        return [this, bus_type](
                       device_io::buses_t const& devices,
                       box<device_io::bus_list_t> const& from_device_list) {
            return (*this)(bus_type, devices, from_device_list);
        };
    }
} const make_mixer_device_routes;

static struct
{
    auto operator()(
            mixer::channel_id const channel_id,
            mixer::io_socket const io_socket,
            mixer::channels_t const& channels) const
            -> boxed_vector<mixer_channel_route>
    {
        auto valid_sources =
                mixer::valid_channels(io_socket, channels, channel_id);
        return box_(algorithm::transform_to_vector(
                valid_sources,
                [&](auto const& id) {
                    return mixer_channel_route{
                            .channel_id = id,
                            .name = channels[id].name};
                }));
    }

    auto operator()(
            mixer::channel_id const channel_id,
            mixer::io_socket const io_socket) const
    {
        return [=, this](mixer::channels_t const& channels) {
            return (*this)(channel_id, io_socket, channels);
        };
    }

} const make_mixer_channel_routes;

static auto
get_default_route_name(
        audio::bus_type const bus_type,
        mixer::io_socket const io_socket) -> boxed_string
{
    static boxed_string const s_none{"None"};
    static boxed_string const s_mix{"Mix"};

    return io_socket == mixer::io_socket::in &&
                           bus_type == audio::bus_type::stereo
                   ? s_mix
                   : s_none;
}

auto
make_mixer_channel_default_route_name_selector(
        audio::bus_type const bus_type,
        mixer::io_socket const io_socket) -> selector<boxed_string>
{
    return boost::hof::always(get_default_route_name(bus_type, io_socket));
}

struct
{
    auto operator()(
            mixer::channel_id const channel_id,
            mixer::io_socket const io_socket,
            mixer::channels_t const& channels,
            device_io::buses_t const& device_buses) const -> box<selected_route>
    {
        mixer::channel const* const mixer_channel = channels.find(channel_id);
        if (!mixer_channel)
        {
            static boxed_string const s_empty{"-"};
            return box_(
                    selected_route{selected_route::state_t::invalid, s_empty});
        }

        auto const default_name =
                get_default_route_name(mixer_channel->bus_type, io_socket);

        return box_(std::visit(
                boost::hof::match(
                        [&](default_t) {
                            return selected_route{
                                    .state = selected_route::state_t::valid,
                                    .name = default_name};
                        },
                        [&](mixer::channel_id const target_channel_id) {
                            if (mixer::channel const* const target_channel =
                                        channels.find(target_channel_id))
                            {
                                return selected_route{
                                        .state =
                                                io_socket == mixer::io_socket::
                                                                        in
                                                        ? selected_route::
                                                                  state_t::valid
                                                : std::holds_alternative<
                                                          default_t>(
                                                          target_channel->in)
                                                        ? selected_route::
                                                                  state_t::valid
                                                        : selected_route::state_t::
                                                                  not_mixed,
                                        .name = target_channel->name};
                            }
                            else
                            {
                                return selected_route{
                                        .state = selected_route::state_t::
                                                invalid,
                                        .name = default_name};
                            }
                        },
                        [&](device_io::bus_id const dev_bus_id) {
                            if (device_io::bus const* const dev_bus =
                                        device_buses.find(dev_bus_id);
                                dev_bus)
                            {
                                return selected_route{
                                        .state = selected_route::state_t::valid,
                                        .name = dev_bus->name};
                            }
                            else
                            {
                                return selected_route{
                                        .state = selected_route::state_t::
                                                invalid,
                                        .name = default_name};
                            }
                        },
                        [](mixer::missing_device_address const& name) {
                            return selected_route{
                                    .state = selected_route::state_t::invalid,
                                    .name = name};
                        }),
                mixer_channel->get_io_addr(io_socket)));
    }

    auto operator()(
            mixer::channel_id const channel_id,
            mixer::io_socket const io_socket) const
    {
        return [=,
                this](mixer::channels_t const& channels,
                      device_io::buses_t const& device_buses) {
            return (*this)(channel_id, io_socket, channels, device_buses);
        };
    }
} const get_mixer_channel_selected_route;

auto
make_mixer_channel_default_route_is_valid_selector(
        mixer::channel_id const channel_id,
        mixer::io_socket const io_socket) -> selector<bool>
{
    switch (io_socket)
    {
        case mixer::io_socket::in:
            return [channel_id, get = memo(&mixer::is_default_source_valid)](
                           state const& st) {
                return get(st.mixer_state.channels, channel_id);
            };

        default:
            return boost::hof::always(true);
    }
}

auto
make_mixer_channel_selected_route_selector(
        mixer::channel_id const channel_id,
        mixer::io_socket const io_socket) -> selector<box<selected_route>>
{
    return [get = memo(
                    get_mixer_channel_selected_route(channel_id, io_socket))](
                   state const& st) {
        return get(st.mixer_state.channels, st.device_io_state.buses);
    };
}

auto
make_mixer_devices_selector(
        audio::bus_type const bus_type,
        mixer::io_socket const io_socket)
        -> selector<boxed_vector<mixer_device_route>>
{
    switch (io_socket)
    {
        case mixer::io_socket::in:
            return [get = memo(make_mixer_device_routes(bus_type))](
                           state const& st) mutable {
                return get(st.device_io_state.buses, st.device_io_state.inputs);
            };

        default:
            return [get = memo(
                            make_mixer_device_routes(audio::bus_type::stereo))](
                           state const& st) mutable {
                return get(
                        st.device_io_state.buses,
                        st.device_io_state.outputs);
            };
    }
}

auto
make_mixer_channels_selector(
        mixer::channel_id const channel_id,
        mixer::io_socket const io_socket)
        -> selector<boxed_vector<mixer_channel_route>>
{
    return [get = memo(make_mixer_channel_routes(channel_id, io_socket))](
                   state const& st) { return get(st.mixer_state.channels); };
}

auto
make_device_bus_name_selector(device_io::bus_id const bus_id)
        -> selector<boxed_string>
{
    auto get_device_bus_name = [](device_io::buses_t const& device_buses,
                                  device_io::bus_id const bus_id) {
        device_io::bus const* const bus = device_buses.find(bus_id);
        return bus ? bus->name : boxed_string();
    };

    return [bus_id,
            get = memo(get_device_bus_name)](state const& st) -> boxed_string {
        return get(st.device_io_state.buses, bus_id);
    };
}

auto
make_mixer_channel_name_selector(mixer::channel_id const channel_id)
        -> selector<boxed_string>
{
    auto get_mixer_channel_name = [](mixer::channels_t const& channels,
                                     mixer::channel_id const channel_id) {
        mixer::channel const* const mixer_channel = channels.find(channel_id);
        return mixer_channel ? mixer_channel->name : boxed_string();
    };

    return [channel_id, get = memo(get_mixer_channel_name)](
                   state const& st) -> boxed_string {
        return get(st.mixer_state.channels, channel_id);
    };
}

auto
make_mixer_channel_can_move_left_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    return [channel_id](state const& st) {
        return channel_id != st.mixer_state.main &&
               !st.mixer_state.inputs->empty() &&
               channel_id != st.mixer_state.inputs->front();
    };
}

auto
make_mixer_channel_can_move_right_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    return [channel_id](state const& st) {
        return channel_id != st.mixer_state.main &&
               !st.mixer_state.inputs->empty() &&
               channel_id != st.mixer_state.inputs->back();
    };
}

auto
make_bus_type_selector(device_io::bus_id const bus_id)
        -> selector<audio::bus_type>
{
    return [bus_id](state const& st) -> audio::bus_type {
        device_io::bus const* bus = st.device_io_state.buses.find(bus_id);
        return bus ? bus->bus_type : audio::bus_type::mono;
    };
}

auto
make_bus_channel_selector(
        device_io::bus_id const bus_id,
        audio::bus_channel const bc) -> selector<std::size_t>
{
    switch (bc)
    {
        case audio::bus_channel::mono:
        case audio::bus_channel::left:
            return [bus_id](state const& st) -> std::size_t {
                device_io::bus const* const bus =
                        st.device_io_state.buses.find(bus_id);
                return bus ? bus->channels.left : piejam::npos;
            };

        case audio::bus_channel::right:
            return [bus_id](state const& st) -> std::size_t {
                device_io::bus const* const bus =
                        st.device_io_state.buses.find(bus_id);
                return bus ? bus->channels.right : piejam::npos;
            };
    }

    BOOST_ASSERT(false);
    __builtin_unreachable();
}

selector<boxed_vector<midi::device_id_t>> const select_midi_input_devices(
        [](state const& st) -> boxed_vector<midi::device_id_t> {
            return st.midi_inputs;
        });

static auto
midi_device_name(
        box<std::unordered_map<midi::device_id_t, midi_device_config>> const&
                midi_devices,
        midi::device_id_t const device_id) -> boxed_string
{
    auto it = midi_devices->find(device_id);
    return it != midi_devices->end() ? it->second.name : boxed_string();
}

auto
make_midi_device_name_selector(midi::device_id_t const device_id)
        -> selector<boxed_string>
{
    return [device_id](state const& st) {
        return midi_device_name(st.midi_devices, device_id);
    };
}

static bool
is_midi_device_enabled(
        box<std::unordered_map<midi::device_id_t, midi_device_config>> const&
                midi_devices,
        midi::device_id_t const device_id)
{
    auto it = midi_devices->find(device_id);
    return it != midi_devices->end() && it->second.enabled;
}

auto
make_midi_device_enabled_selector(midi::device_id_t const device_id)
        -> selector<bool>
{
    return [device_id](state const& st) {
        return is_midi_device_enabled(st.midi_devices, device_id);
    };
}

using muted_by_solo_state_t = std::
        unordered_map<mixer::channel_id, std::function<bool(state const&)>>;

static auto
make_muted_by_solo_state(mixer::channels_t const& channels)
{
    muted_by_solo_state_t result;

    for (auto&& [owner, group] : runtime::solo_groups(channels))
    {
        auto group_selectors = algorithm::transform_to_vector(
                group,
                [](solo_group_member const& sg_member) {
                    return make_bool_parameter_value_selector(
                            sg_member.solo_param);
                });

        for (auto&& [param, member] : group)
        {
            result.emplace(
                    member,
                    [group_selectors,
                     member_selector = make_bool_parameter_value_selector(
                             param)](state const& st) {
                        return std::ranges::any_of(
                                       group_selectors,
                                       [&st](auto const& sel) {
                                           return sel.get(st);
                                       }) &&
                               !member_selector.get(st);
                    });
        }
    }

    return result;
}

selector<box<muted_by_solo_state_t>> select_muted_by_solo_state(
        [get = memo(boxify_result(&make_muted_by_solo_state))](
                state const& st) { return get(st.mixer_state.channels); });

auto
make_muted_by_solo_selector(mixer::channel_id const mixer_channel_id)
        -> selector<bool>
{
    return [mixer_channel_id](state const& st) {
        auto const& muted_by_solo = select_muted_by_solo_state.get(st);
        auto it = muted_by_solo->find(mixer_channel_id);
        return it != muted_by_solo->end() && it->second(st);
    };
}

auto
make_fx_chain_selector(mixer::channel_id mixer_channel_id)
        -> selector<box<fx::chain_t>>
{
    return [mixer_channel_id](state const& st) -> box<fx::chain_t> {
        static box<fx::chain_t> s_empty_fx_chain;
        mixer::channel const* const mixer_channel =
                st.mixer_state.channels.find(mixer_channel_id);
        return mixer_channel ? mixer_channel->fx_chain : s_empty_fx_chain;
    };
}

auto
make_fx_module_instance_id_selector(fx::module_id const fx_mod_id)
        -> selector<fx::instance_id>
{
    return [fx_mod_id](state const& st) -> fx::instance_id {
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->fx_instance_id : fx::instance_id();
    };
}

auto
make_fx_module_name_selector(fx::module_id const fx_mod_id)
        -> selector<boxed_string>
{
    return [fx_mod_id](state const& st) -> boxed_string {
        static boxed_string s_empty_name;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->name : s_empty_name;
    };
}

auto
make_fx_module_bus_type_selector(fx::module_id const fx_mod_id)
        -> selector<audio::bus_type>
{
    return [fx_mod_id](state const& st) -> audio::bus_type {
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->bus_type : audio::bus_type::mono;
    };
}

auto
make_fx_module_bypass_selector(fx::module_id const fx_mod_id) -> selector<bool>
{
    return [fx_mod_id](state const& st) -> bool {
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod && fx_mod->bypassed;
    };
}

auto
make_fx_module_parameters_selector(fx::module_id const fx_mod_id)
        -> selector<box<fx::module_parameters>>
{
    return [fx_mod_id](state const& st) -> box<fx::module_parameters> {
        static box<fx::module_parameters> s_empty;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->parameters : s_empty;
    };
}

auto
make_fx_module_can_move_up_selector(mixer::channel_id const fx_chain_id)
        -> selector<bool>
{
    return [=](state const& st) -> bool {
        if (fx_chain_id != st.gui_state.focused_fx_chain_id)
        {
            return false;
        }

        mixer::channel const* const mixer_channel =
                st.mixer_state.channels.find(fx_chain_id);
        return mixer_channel && !mixer_channel->fx_chain->empty() &&
               mixer_channel->fx_chain->front() !=
                       st.gui_state.focused_fx_mod_id;
    };
}

auto
make_fx_module_can_move_down_selector(mixer::channel_id const fx_chain_id)
        -> selector<bool>
{
    return [=](state const& st) -> bool {
        if (fx_chain_id != st.gui_state.focused_fx_chain_id)
        {
            return false;
        }

        mixer::channel const* const mixer_channel =
                st.mixer_state.channels.find(fx_chain_id);
        return mixer_channel && !mixer_channel->fx_chain->empty() &&
               mixer_channel->fx_chain->back() !=
                       st.gui_state.focused_fx_mod_id;
    };
}

auto
make_fx_parameter_name_selector(fx::parameter_id const fx_param_id)
        -> selector<boxed_string>
{
    return [fx_param_id](state const& st) -> boxed_string {
        static boxed_string s_empty;
        return std::visit(
                [&](auto param_id) {
                    if (auto const* const desc = st.ui_params.find(param_id);
                        desc)
                    {
                        return desc->name;
                    }
                    return s_empty;
                },
                fx_param_id);
    };
}

auto
make_fx_parameter_value_string_selector(fx::parameter_id const fx_param_id)
        -> selector<std::string>
{
    return [fx_param_id](state const& st) -> std::string {
        return std::visit(
                [&](auto param_id) {
                    auto const* const desc = st.params.find(param_id);
                    auto const* const ui_desc =
                            desc ? st.ui_params.find(param_id) : nullptr;
                    return desc && ui_desc
                                   ? ui_desc->value_to_string(desc->value.get())
                                   : std::string{};
                },
                fx_param_id);
    };
}

auto
make_fx_module_streams_selector(fx::module_id fx_mod_id)
        -> selector<box<fx::module_streams>>
{
    return [fx_mod_id](state const& st) -> box<fx::module_streams> {
        static box<fx::module_streams> s_empty;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->streams : s_empty;
    };
}

auto
make_audio_stream_selector(audio_stream_id stream_id)
        -> selector<audio_stream_buffer>
{
    return [stream_id](state const& st) -> audio_stream_buffer {
        static audio_stream_buffer s_empty(std::in_place, 1);
        auto* stream = st.streams.find(stream_id);
        return stream ? *stream : s_empty;
    };
}

template <class P>
auto
make_parameter_value_selector(parameter::id_t<P> const param_id)
        -> selector<parameter::value_type_t<P>>
{
    return [param_id,
            value = std::shared_ptr<parameter::value_type_t<P> const>{}](
                   state const& st) mutable -> parameter::value_type_t<P> {
        if (value) [[likely]]
        {
            return *value;
        }

        if (auto const* const desc = st.params.find(param_id); desc)
        {
            value = desc->value.cached();
            BOOST_ASSERT(value);
            return *value;
        }

        return {};
    };
}

auto
make_bool_parameter_value_selector(bool_parameter_id const param_id)
        -> selector<bool>
{
    return make_parameter_value_selector(param_id);
}

auto
make_float_parameter_value_selector(float_parameter_id const param_id)
        -> selector<float>
{
    return make_parameter_value_selector(param_id);
}

auto
make_float_parameter_normalized_value_selector(
        float_parameter_id const param_id) -> selector<float>
{
    return [param_id](state const& st) -> float {
        if (auto const* const desc = st.params.find(param_id); desc)
        {
            float const value = desc->value.get();
            BOOST_ASSERT(desc->param.to_normalized);
            return desc->param.to_normalized(desc->param, value);
        }
        return 0.f;
    };
}

auto
make_int_parameter_value_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return make_parameter_value_selector(param_id);
}

auto
make_int_parameter_min_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const desc = st.params.find(param_id);
        return desc ? desc->param.min : 0;
    };
}

auto
make_int_parameter_max_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const desc = st.params.find(param_id);
        return desc ? desc->param.max : 1;
    };
}

auto
make_int_parameter_enum_values_selector(int_parameter_id const param_id)
        -> selector<std::vector<std::pair<std::string, int>>>
{
    return [param_id](state const& st) {
        std::vector<std::pair<std::string, int>> result;

        auto const* const desc = st.params.find(param_id);
        auto const* const ui_desc = st.ui_params.find(param_id);
        if (desc && ui_desc)
        {
            for (int value = desc->param.min; value <= desc->param.max; ++value)
            {
                result.emplace_back(ui_desc->value_to_string(value), value);
            }
        }

        return result;
    };
}

auto
make_level_parameter_value_selector(stereo_level_parameter_id const param_id)
        -> selector<stereo_level>
{
    return make_parameter_value_selector(param_id);
}

auto
make_midi_assignment_selector(midi_assignment_id const id)
        -> selector<std::optional<midi_assignment>>
{
    return [id](state const& st) -> std::optional<midi_assignment> {
        auto it = st.midi_assignments->find(id);
        return it != st.midi_assignments->end()
                       ? it->second
                       : std::optional<midi_assignment>{};
    };
}

selector<bool> const select_midi_learning([](state const& st) {
    return st.midi_learning.has_value();
});

selector<fx::registry> const select_fx_registry([](state const& st) {
    return st.fx_registry;
});

selector<bool> const select_recording([](state const& st) {
    return st.recording;
});

selector<std::size_t> const select_xruns([](state const& st) {
    return st.xruns;
});

selector<float> const select_cpu_load([](state const& st) {
    return st.cpu_load;
});

selector<root_view_mode> const select_root_view_mode([](state const& st) {
    return st.gui_state.root_view_mode_;
});

selector<mixer::channel_id> const select_fx_browser_fx_chain(
        [](state const& st) { return st.gui_state.fx_browser_fx_chain_id; });

selector<mixer::channel_id> const select_focused_fx_chain([](state const& st) {
    return st.gui_state.focused_fx_chain_id;
});

selector<fx::module_id> const select_focused_fx_module([](state const& st) {
    return st.gui_state.focused_fx_mod_id;
});

selector<boxed_string> const select_focused_fx_module_name([](state const& st) {
    static boxed_string s_empty_name;

    fx::module const* const fx_mod =
            st.fx_modules.find(st.gui_state.focused_fx_mod_id);
    return fx_mod ? fx_mod->name : s_empty_name;
});

selector<bool> const
select_focused_fx_module_bypassed([](state const& st) -> bool {
    fx::module const* const fx_mod =
            st.fx_modules.find(st.gui_state.focused_fx_mod_id);
    return fx_mod && fx_mod->bypassed;
});

} // namespace piejam::runtime::selectors
