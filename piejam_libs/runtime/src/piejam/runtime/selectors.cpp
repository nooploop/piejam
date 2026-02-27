// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/selectors.h>

#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/solo_group.h>
#include <piejam/runtime/state.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/multichannel_buffer.h>
#include <piejam/functional/memo.h>
#include <piejam/npos.h>
#include <piejam/range/indirected.h>
#include <piejam/redux/selector.h>

#include <boost/hof/match.hpp>
#include <boost/hof/unpack.hpp>

namespace piejam::runtime::selectors
{

namespace
{

template <
    std::invocable<state const&> GetDataMap,
    std::invocable<state const&> GetId,
    class Data>
    requires(std::is_same_v<
             std::invoke_result_t<GetDataMap, state const&>,
             entity_data_map<
                 std::invoke_result_t<GetId, state const&>,
                 std::decay_t<Data>> const&>)
auto
make_entity_data_map_selector(
    GetDataMap&& get_data_map,
    GetId&& get_id,
    Data&& default_data) -> selector<std::decay_t<Data>>
{
    return [get_data_map = std::forward<GetDataMap>(get_data_map),
            get_id = std::forward<GetId>(get_id),
            default_data = std::forward<Data>(default_data),
            cached = cached_entity_data_ptr<std::decay_t<Data>>{}](
               state const& st) mutable {
        if (cached) [[likely]]
        {
            return *cached;
        }

        auto const& data_map = std::invoke(get_data_map, st);
        cached = data_map.cached(std::invoke(get_id, st));
        return cached ? *cached : default_data;
    };
}

} // namespace

auto
make_string_selector(string_id id) -> selector<boxed_string>
{
    return make_entity_data_map_selector(
        &state::strings,
        boost::hof::always(id),
        boxed_string{});
}

selector<box<sample_rate_choice>> const select_sample_rate([](state const& st) {
    static auto const get_sample_rate =
        shared_memo([](box<audio::sound_card_hw_params> const& hw_params,
                       audio::sample_rate const current) {
            return box<sample_rate_choice>{
                std::in_place,
                hw_params->sample_rates,
                current};
        });

    return get_sample_rate(st.selected_sound_card.hw_params, st.sample_rate);
});

selector<box<period_size_choice>> const select_period_size([](state const& st) {
    static auto const get_period_size =
        shared_memo([](box<audio::sound_card_hw_params> const& hw_params,
                       audio::period_size const current) {
            return box<period_size_choice>{
                std::in_place,
                hw_params->period_sizes,
                current};
        });

    return get_period_size(st.selected_sound_card.hw_params, st.period_size);
});

selector<float> const select_buffer_latency([](state const& st) {
    return st.sample_rate.value() != 0
               ? st.sample_rate
                     .duration_for_samples<std::milli, float>(
                         st.period_size.value())
                     .count()
               : 0.f;
});

selector<box<sound_card_choice>> const select_sound_card([](state const& st) {
    static auto get_sound_card = shared_memo(
        [](box<audio::sound_cards> const& descs, std::size_t const index) {
            return box<sound_card_choice>{
                std::in_place,
                algorithm::transform_to_vector(
                    descs.get(),
                    [](auto const& desc) {
                        return sound_card_info{
                            .name = desc.name,
                            .num_ins = desc.num_channels.in(),
                            .num_outs = desc.num_channels.out(),
                        };
                    }),
                index};
        });
    return get_sound_card(st.sound_cards, st.selected_sound_card.index);
});

auto
make_num_device_channels_selector(io_direction const io_dir)
    -> selector<std::size_t>
{
    return selector<std::size_t>([io_dir](state const& st) -> std::size_t {
        return st.selected_sound_card.num_channels[io_dir];
    });
}

auto
make_external_audio_device_ids_selector(io_direction const io_dir)
    -> selector<box<external_audio::device_ids_t>>
{
    return [io_dir](state const& st) {
        return st.external_audio_state.io_ids[io_dir];
    };
}

template <auto GetMember>
static auto
make_external_audio_member_selector(
    external_audio::device_id const device_id,
    decltype(std::invoke(
        GetMember,
        std::declval<external_audio::device const&>())) default_value = {})
{
    auto get =
        shared_memo([device_id, default_value = std::move(default_value)](
                        external_audio::devices_t const& devices) {
            external_audio::device const* const device =
                devices.find(device_id);
            return device ? std::invoke(GetMember, *device) : default_value;
        });

    return [get = std::move(get)](state const& st) {
        return get(st.external_audio_state.devices);
    };
}

auto
make_external_audio_device_name_selector(
    external_audio::device_id const device_id) -> selector<string_id>
{
    return make_external_audio_member_selector<&external_audio::device::name>(
        device_id);
}

static auto
make_external_audio_device_name_string_selector(
    external_audio::device_id const device_id) -> selector<boxed_string>
{
    return make_entity_data_map_selector(
        &state::strings,
        make_external_audio_device_name_selector(device_id),
        boxed_string{});
}

auto
make_external_audio_device_bus_type_selector(
    external_audio::device_id const device_id) -> selector<audio::bus_type>
{
    return make_external_audio_member_selector<
        &external_audio::device::bus_type>(device_id);
}

auto
make_external_audio_device_bus_channel_selector(
    external_audio::device_id const device_id,
    audio::bus_channel const bus_channel) -> selector<std::size_t>
{
    auto get = shared_memo(
        [device_id, bus_channel](external_audio::device_channels_t const& m) {
            auto const* const ch = m.find({device_id, bus_channel});
            return ch ? *ch : npos;
        });

    return [get = std::move(get)](state const& st) {
        return get(st.external_audio_state.device_channels);
    };
}

static auto
get_mixer_aux_channels(mixer::aux_channels_t const& aux_channels)
    -> box<mixer::channel_ids_t>
{
    return box{
        aux_channels | std::views::keys | std::ranges::to<std::vector>()};
}

selector<box<mixer::channel_ids_t>> const select_mixer_aux_channels(
    [get = shared_memo(&get_mixer_aux_channels)](state const& st) {
        return get(st.mixer_state.aux_channels);
    });

selector<box<mixer::channel_ids_t>> const select_mixer_user_channels(
    [](state const& st) { return st.mixer_state.inputs; });

selector<mixer::channel_id> const select_mixer_main_channel(
    [](state const& st) { return st.mixer_state.main; });

template <auto GetMember>
static auto
make_mixer_channel_member_selector(
    mixer::channel_id const channel_id,
    decltype(std::invoke(GetMember, std::declval<mixer::channel const&>()))
        default_value = {})
{
    auto get =
        shared_memo([channel_id, default_value = std::move(default_value)](
                        mixer::channels_t const& channels) {
            mixer::channel const* const channel = channels.find(channel_id);
            return channel ? std::invoke(GetMember, *channel) : default_value;
        });

    return [get = std::move(get)](state const& st) {
        return get(st.mixer_state.channels);
    };
}

auto
make_mixer_channel_type_selector(mixer::channel_id const channel_id)
    -> selector<mixer::channel_type>
{
    return [channel_id](state const& st) {
        return st.mixer_state.channels.at(channel_id).type;
    };
}

auto
make_mixer_channel_color_selector(mixer::channel_id const channel_id)
    -> selector<material_color>
{
    return make_entity_data_map_selector(
        [](state const& st) -> auto& { return st.material_colors; },
        [channel_id](state const& st) {
            auto channel = st.mixer_state.channels.find(channel_id);
            return channel ? channel->color : material_color_id{};
        },
        material_color::pink);
}

auto
make_mixer_channel_volume_parameter_selector(mixer::channel_id const channel_id)
    -> selector<float_parameter_id>
{
    return [channel_id](state const& st) {
        return st.mixer_state.channels.at(channel_id).volume();
    };
}

auto
make_mixer_channel_pan_balance_parameter_selector(
    mixer::channel_id const channel_id) -> selector<float_parameter_id>
{
    return [channel_id](state const& st) {
        return st.mixer_state.channels.at(channel_id).pan_balance();
    };
}

auto
make_mixer_channel_record_parameter_selector(mixer::channel_id const channel_id)
    -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        return st.mixer_state.channels.at(channel_id).record();
    };
}

auto
make_mixer_channel_mute_parameter_selector(mixer::channel_id const channel_id)
    -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        return st.mixer_state.channels.at(channel_id).mute();
    };
}

auto
make_mixer_channel_solo_parameter_selector(mixer::channel_id const channel_id)
    -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        return st.mixer_state.channels.at(channel_id).solo();
    };
}

auto
make_mixer_channel_out_stream_selector(mixer::channel_id const channel_id)
    -> selector<audio_stream_id>
{
    return [channel_id](state const& st) {
        return st.mixer_state.channels.at(channel_id).out_stream;
    };
}

auto
make_aux_send_volume_parameter_selector(
    mixer::channel_id const channel_id,
    mixer::channel_id const aux_id) -> selector<float_parameter_id>
{
    return [=](state const& st) {
        return st.mixer_state.aux_sends.at(channel_id).at(aux_id).volume();
    };
}

auto
make_aux_send_active_selector(
    mixer::channel_id const channel_id,
    mixer::channel_id const aux_id) -> selector<bool_parameter_id>
{
    return [=](state const& st) {
        return st.mixer_state.aux_sends.at(channel_id).at(aux_id).active();
    };
}

auto
make_aux_send_fader_tap_selector(
    mixer::channel_id const channel_id,
    mixer::channel_id const aux_id) -> selector<enum_parameter_id>
{
    return [=](state const& st) {
        return st.mixer_state.aux_sends.at(channel_id).at(aux_id).fader_tap();
    };
}

auto
make_can_toggle_aux_send_selector(
    mixer::channel_id const channel_id,
    mixer::channel_id const aux_id) -> selector<bool>
{
    auto get = shared_memo([channel_id, aux_id](
                               mixer::io_map_t const& io_map,
                               mixer::aux_sends_t const& aux_sends,
                               parameter::store const& params) {
        return mixer::can_toggle_aux(
            channel_id,
            aux_id,
            io_map,
            aux_sends,
            params);
    });

    return [get = std::move(get)](state const& st) {
        return get(st.mixer_state.io_map, st.mixer_state.aux_sends, st.params);
    };
}

auto
make_aux_channel_default_fader_tap_parameter_selector(mixer::channel_id aux_id)
    -> selector<enum_parameter_id>
{
    return [aux_id](state const& st) {
        return st.mixer_state.aux_channels.at(aux_id).default_fader_tap();
    };
}

auto
make_mixer_channel_mix_input_is_valid_selector(
    mixer::channel_id const channel_id) -> selector<bool>
{
    return [channel_id,
            get = shared_memo(&mixer::is_mix_input_valid)](state const& st) {
        return get(
            channel_id,
            st.mixer_state.io_map,
            st.mixer_state.aux_sends,
            st.params);
    };
}

static auto
make_route_state_selector(mixer::io_address_t addr, io_direction io_socket)
    -> selector<selected_route::state_t>
{
    return std::visit(
        boost::hof::match(
            [io_socket](
                mixer::channel_id id) -> selector<selected_route::state_t> {
                if (io_socket == io_direction::input)
                {
                    return boost::hof::always(selected_route::state_t::valid);
                }
                else
                {
                    return [id](state const& st) {
                        auto addr = st.mixer_state.io_map.at(id).in();
                        return std::holds_alternative<mixer::mix_input>(addr)
                                   ? selected_route::state_t::valid
                                   : selected_route::state_t::not_mixed;
                    };
                }
            },
            [](auto) -> selector<selected_route::state_t> {
                return boost::hof::always(selected_route::state_t::valid);
            }),
        addr);
}

static auto
make_route_name_selector(mixer::io_address_t addr) -> selector<boxed_string>
{
    using namespace std::string_literals;
    return std::visit(
        boost::hof::match(
            [](default_t) -> selector<boxed_string> {
                static boxed_string s_none{"None"s};
                return boost::hof::always(s_none);
            },
            [](mixer::mix_input) -> selector<boxed_string> {
                static boxed_string s_mix{"Mix"s};
                return boost::hof::always(s_mix);
            },
            [](external_audio::device_id id) -> selector<boxed_string> {
                return make_external_audio_device_name_string_selector(id);
            },
            [](mixer::channel_id id) -> selector<boxed_string> {
                return make_mixer_channel_name_string_selector(id);
            }),
        addr);
}

auto
make_mixer_channel_selected_route_selector(
    mixer::channel_id const channel_id,
    io_direction const io_dir) -> selector<selected_route>
{
    return
        [io_dir,
         channel_id,
         get_state = shared_memo(&make_route_state_selector),
         get_name = shared_memo(&make_route_name_selector)](state const& st) {
            auto addr = st.mixer_state.io_map.at(channel_id)[io_dir];

            selected_route result;

            result.state = get_state(addr, io_dir)(st);
            result.name = get_name(addr)(st);

            return result;
        };
}

auto
make_mixer_device_routes_selector(
    mixer::channel_type const channel_type,
    io_direction const io_port) -> selector<boxed_vector<mixer_device_route>>
{
    if (channel_type == mixer::channel_type::aux &&
        io_port == io_direction::input)
    {
        return boost::hof::always(boxed_vector<mixer_device_route>{});
    }

    auto const bus_type = (io_port != io_direction::output &&
                           channel_type == mixer::channel_type::mono)
                              ? audio::bus_type::mono
                              : audio::bus_type::stereo;

    auto get_mixer_device_routes = shared_memo(
        [bus_type](
            external_audio::devices_t const& devices,
            box<external_audio::device_ids_t> const& device_ids)
            -> boxed_vector<mixer_device_route> {
            std::vector<mixer_device_route> result;
            for (auto device_id : *device_ids)
            {
                if (auto const& device = devices.at(device_id);
                    device.bus_type == bus_type)
                {
                    result.emplace_back(
                        mixer_device_route{
                            .device_id = device_id,
                            .name = device.name});
                }
            }
            return box(std::move(result));
        });

    return [io_port,
            get = std::move(get_mixer_device_routes)](state const& st) mutable {
        return get(
            st.external_audio_state.devices,
            st.external_audio_state.io_ids[io_port]);
    };
}

auto
make_mixer_channel_routes_selector(
    mixer::channel_id const channel_id,
    io_direction const io_port) -> selector<boxed_vector<mixer_channel_route>>
{
    auto get_mixer_channel_routes =
        shared_memo([channel_id, io_port](
                        mixer::channels_t const& channels,
                        mixer::io_map_t const& io_map,
                        mixer::aux_sends_t const& aux_sends,
                        parameter::store const& params) {
            if (io_port == io_direction::input &&
                channels.at(channel_id).type == mixer::channel_type::aux)
            {
                return boxed_vector<mixer_channel_route>{};
            }

            auto valid_sources = mixer::valid_channels(
                channel_id,
                io_port,
                channels,
                io_map,
                aux_sends,
                params);
            return box(
                algorithm::transform_to_vector(
                    valid_sources,
                    [&](auto const& id) {
                        return mixer_channel_route{
                            .channel_id = id,
                            .name = channels.at(id).name};
                    }));
        });

    return [get = std::move(get_mixer_channel_routes)](state const& st) {
        return get(
            st.mixer_state.channels,
            st.mixer_state.io_map,
            st.mixer_state.aux_sends,
            st.params);
    };
}

auto
make_mixer_channel_name_selector(mixer::channel_id const channel_id)
    -> selector<string_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::name>(
        channel_id);
}

auto
make_mixer_channel_name_string_selector(mixer::channel_id const channel_id)
    -> selector<boxed_string>
{
    return make_entity_data_map_selector(
        &state::strings,
        make_mixer_channel_name_selector(channel_id),
        boxed_string{});
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

selector<box<midi::device_ids_t>> const
    select_midi_input_devices([](state const& st) { return st.midi_inputs; });

auto
make_midi_device_name_selector(midi::device_id_t const device_id)
    -> selector<boxed_string>
{
    auto get_midi_device_name =
        shared_memo([device_id](box<midi_devices_t> const& midi_devices) {
            auto it = midi_devices->find(device_id);
            return it != midi_devices->end() ? it->second.name : boxed_string();
        });

    return [get = std::move(get_midi_device_name)](state const& st) {
        return get(st.midi_devices);
    };
}

auto
make_midi_device_enabled_selector(midi::device_id_t const device_id)
    -> selector<bool>
{
    auto is_midi_device_enabled =
        shared_memo([device_id](box<midi_devices_t> const& midi_devices) {
            auto it = midi_devices->find(device_id);
            return it != midi_devices->end() && it->second.enabled;
        });

    return [get = std::move(is_midi_device_enabled)](state const& st) {
        return get(st.midi_devices);
    };
}

namespace
{

struct muted_by_solo_state
{
    muted_by_solo_state(
        mixer::channels_t const& channels,
        mixer::io_map_t const& io_map,
        mixer::aux_sends_t const& aux_sends,
        parameter::store const& params)
        : solo_groups{runtime::solo_groups(channels, io_map, aux_sends, params)}
        , solo_params{algorithm::transform_to_vector(
              solo_groups | std::views::values,
              [params](solo_group const& g) {
                  return params.at(g.solo_param).cached();
              })}
    {
    }

    auto operator()(
        std::size_t const update_count,
        mixer::channel_id const channel_id) const
    {
        if (last_update_count != update_count)
        {
            for (auto [index, solo] :
                 solo_params | range::indirected | std::views::enumerate)
            {
                sg_state.set_solo(index, solo);
            }

            sg_state.calculate_mutes();
            last_update_count = update_count;
        }

        auto it = solo_groups.find(channel_id);
        return it != solo_groups.end() &&
               sg_state.mutes().test(solo_groups.index_of(it));
    }

    solo_groups_t solo_groups;
    mutable solo_group_state sg_state{solo_groups};

    using cached_solo_param_value = std::shared_ptr<bool const>;
    std::vector<cached_solo_param_value> solo_params;

    mutable std::size_t last_update_count{piejam::npos};
};

auto
make_muted_by_solo_state(
    mixer::channels_t const& channels,
    mixer::io_map_t const& io_map,
    mixer::aux_sends_t const& aux_sends,
    parameter::store const& params) -> box<muted_by_solo_state>
{
    return box(muted_by_solo_state{channels, io_map, aux_sends, params});
}

selector<box<muted_by_solo_state>> const select_muted_by_solo_state(
    [get = shared_memo(&make_muted_by_solo_state)](state const& st) {
        return get(
            st.mixer_state.channels,
            st.mixer_state.io_map,
            st.mixer_state.aux_sends,
            st.params);
    });

} // namespace

auto
make_muted_by_solo_selector(mixer::channel_id const mixer_channel_id)
    -> selector<bool>
{
    return [mixer_channel_id](state const& st) {
        auto muted_by_solo_state = select_muted_by_solo_state(st);
        return muted_by_solo_state.get()(
            st.solo_state_update_count,
            mixer_channel_id);
    };
}

auto
make_fx_chain_selector(mixer::channel_id const channel_id)
    -> selector<box<fx::chain_t>>
{
    auto get =
        shared_memo([channel_id](mixer::state::fx_chains_t const& fx_chains) {
            return box{fx_chains.at(channel_id)};
        });

    return [get = std::move(get)](state const& st) {
        return get(st.mixer_state.fx_chains);
    };
}

template <auto GetMember>
auto
make_fx_module_member_selector(fx::module_id const fx_mod_id)
{
    static decltype(std::invoke(
        GetMember,
        std::declval<fx::module const*>())) s_default{};

    auto get = shared_memo([fx_mod_id](fx::modules_t const& fx_modules) {
        fx::module const* const fx_mod = fx_modules.find(fx_mod_id);
        return fx_mod ? std::invoke(GetMember, fx_mod) : s_default;
    });

    return [get = std::move(get)](state const& st) {
        return get(st.fx_state.modules);
    };
}

auto
make_fx_module_instance_id_selector(fx::module_id const fx_mod_id)
    -> selector<fx::instance_id>
{
    return [fx_mod_id](state const& st) {
        return st.fx_state.modules.at(fx_mod_id).fx_instance_id;
    };
}

auto
make_fx_module_name_selector(fx::module_id const fx_mod_id)
    -> selector<boxed_string>
{
    return [fx_mod_id](state const& st) {
        return st.fx_state.modules.at(fx_mod_id).name;
    };
}

auto
make_fx_module_bus_type_selector(fx::module_id const fx_mod_id)
    -> selector<audio::bus_type>
{
    return [fx_mod_id](state const& st) {
        return st.fx_state.modules.at(fx_mod_id).bus_type;
    };
}

auto
make_fx_module_active_selector(fx::module_id const fx_mod_id)
    -> selector<bool_parameter_id>
{
    return [fx_mod_id](state const& st) {
        return st.fx_state.active_modules.at(fx_mod_id);
    };
}

auto
make_fx_module_parameters_selector(fx::module_id const fx_mod_id)
    -> selector<box<parameters_map>>
{
    return [fx_mod_id](state const& st) {
        return st.fx_state.modules.at(fx_mod_id).parameters;
    };
}

auto
make_fx_module_streams_selector(fx::module_id fx_mod_id)
    -> selector<box<fx::module_streams>>
{
    return [fx_mod_id](state const& st) {
        return st.fx_state.modules.at(fx_mod_id).streams;
    };
}

static auto
make_fx_module_can_move_selector(mixer::channel_id const fx_chain_id, bool up)
    -> selector<bool>
{
    auto get_fx_chain =
        shared_memo([fx_chain_id](mixer::state::fx_chains_t const& fx_chains) {
            return box{fx_chains.at(fx_chain_id)};
        });

    return [fx_chain_id, up, get_fx_chain = std::move(get_fx_chain)](
               state const& st) -> bool {
        if (fx_chain_id != st.focused_fx_chain_id)
        {
            return false;
        }

        auto fx_chain = get_fx_chain(st.mixer_state.fx_chains);

        return !fx_chain->empty() &&
               (up ? fx_chain->front() : fx_chain->back()) !=
                   st.focused_fx_mod_id;
    };
}

auto
make_fx_module_can_move_up_selector(mixer::channel_id const fx_chain_id)
    -> selector<bool>
{
    return make_fx_module_can_move_selector(fx_chain_id, true);
}

auto
make_fx_module_can_move_down_selector(mixer::channel_id const fx_chain_id)
    -> selector<bool>
{
    return make_fx_module_can_move_selector(fx_chain_id, false);
}

auto
make_parameter_name_selector(parameter_id const param_id)
    -> selector<boxed_string>
{
    return std::visit(
        [](auto param_id) -> selector<boxed_string> {
            return [param_id](state const& st) {
                return st.params.at(param_id).param().name;
            };
        },
        param_id);
}

auto
make_parameter_value_string_selector(parameter_id const param_id)
    -> selector<std::string>
{
    return std::visit(
        []<class P>(parameter::id_t<P> param_id) -> selector<std::string> {
            using cached_value_type =
                typename parameter::store::slot<P>::cached_type;

            using value_to_string_f = typename P::value_to_string_f;
            using memoed_value_to_string_f =
                decltype(shared_memo(std::declval<value_to_string_f>()));

            return
                [param_id,
                 cached_value = cached_value_type{},
                 value_to_string = std::optional<memoed_value_to_string_f>{}](
                    state const& st) mutable {
                    if (!cached_value || !value_to_string) [[unlikely]]
                    {
                        auto const slot = st.params.find(param_id);
                        if (!slot)
                        {
                            return std::string{};
                        }

                        cached_value = slot->cached();
                        BOOST_ASSERT(cached_value);
                        value_to_string =
                            shared_memo(slot->param().value_to_string);
                    }

                    return (*value_to_string)(*cached_value);
                };
        },
        param_id);
}

auto
make_audio_stream_selector(audio_stream_id stream_id)
    -> selector<audio_stream_buffer>
{
    static audio_stream_buffer s_empty(std::in_place, 1);

    return make_entity_data_map_selector(
        &state::streams,
        boost::hof::always(stream_id),
        s_empty);
}

auto
make_float_parameter_bipolar_selector(float_parameter_id const fx_param_id)
    -> selector<bool>
{
    return [fx_param_id](state const& st) {
        return st.params.at(fx_param_id)
            .param()
            .flags.test(parameter_flags::bipolar);
    };
}

auto
make_enum_parameter_values_selector(enum_parameter_id const param_id)
    -> selector<std::vector<std::pair<std::string, int>>>
{
    return [param_id](state const& st) {
        std::vector<std::pair<std::string, int>> result;

        auto const& slot = st.params.at(param_id);

        for (int value = slot.param().min; value <= slot.param().max; ++value)
        {
            result.emplace_back(slot.param().value_to_string(value), value);
        }

        return result;
    };
}

template <class P>
auto
make_parameter_min_selector(parameter::id_t<P> param_id)
    -> selector<parameter::value_type_t<P>>
{
    return [param_id](state const& st) {
        return st.params.at(param_id).param().min;
    };
}

template auto make_parameter_min_selector(int_parameter_id) -> selector<int>;
template auto make_parameter_min_selector(enum_parameter_id) -> selector<int>;

template <class P>
auto
make_parameter_max_selector(parameter::id_t<P> param_id)
    -> selector<parameter::value_type_t<P>>
{
    return [param_id](state const& st) {
        return st.params.at(param_id).param().max;
    };
}

template auto make_parameter_max_selector(int_parameter_id) -> selector<int>;
template auto make_parameter_max_selector(enum_parameter_id) -> selector<int>;

template <class P>
auto
make_parameter_value_selector(parameter::id_t<P> const param_id)
    -> selector<parameter::value_type_t<P>>
{
    return [param_id,
            cached = std::shared_ptr<parameter::value_type_t<P> const>{}](
               state const& st) mutable -> parameter::value_type_t<P> {
        if (cached) [[likely]]
        {
            return *cached;
        }

        if (auto const* const desc = st.params.find(param_id); desc)
        {
            cached = desc->cached();
            BOOST_ASSERT(cached);
            return *cached;
        }

        return {};
    };
}

template auto make_parameter_value_selector(float_parameter_id)
    -> selector<float>;
template auto make_parameter_value_selector(bool_parameter_id)
    -> selector<bool>;
template auto make_parameter_value_selector(int_parameter_id) -> selector<int>;
template auto make_parameter_value_selector(enum_parameter_id) -> selector<int>;

template <class P>
auto
make_parameter_normalized_value_selector(parameter::id_t<P> const param_id)
    -> selector<float>
{
    auto get = shared_memo([param_id](
                               parameter::store const& params,
                               parameter::value_type_t<P> value) {
        if (auto const* const slot = params.find(param_id); slot)
        {
            BOOST_ASSERT(slot->param().to_normalized);
            return slot->param().to_normalized(slot->param(), value);
        }

        return 0.f;
    });

    return [get_value = make_parameter_value_selector(param_id),
            get = std::move(get)](state const& st) {
        return get(st.params, get_value(st));
    };
}

template auto make_parameter_normalized_value_selector(float_parameter_id)
    -> selector<float>;

auto
make_parameter_is_midi_assignable_selector(parameter_id param_id)
    -> selector<bool>
{
    return std::visit(
        [](auto const id) -> selector<bool> {
            return [id](state const& st) -> bool {
                return !st.params.at(id).param().flags.test(
                    parameter_flags::not_midi_assignable);
            };
        },
        param_id);
}

auto
make_is_midi_learning_selector(parameter_id const id) -> selector<bool>
{
    return [id](state const& st) { return st.midi_learning == id; };
}

auto
make_midi_assignment_selector(parameter_id const id)
    -> selector<std::optional<midi_assignment>>
{
    auto get = shared_memo([id](midi_assignments_map const& midi_assigns) {
        auto it = midi_assigns.find(id);
        return it != midi_assigns.end() ? it->second
                                        : std::optional<midi_assignment>{};
    });

    return [get = std::move(get)](state const& st) {
        return get(*st.midi_assignments);
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

selector<std::size_t> const select_display_rotation([](state const& st) {
    return st.display_rotation;
});

selector<bool> const select_on_screen_keyboard_enabled([](state const& st) {
    return st.on_screen_keyboard_enabled;
});

selector<root_view_mode> const select_root_view_mode([](state const& st) {
    return st.root_view_mode;
});

selector<mixer::channel_id> const select_fx_browser_fx_chain(
    [](state const& st) { return st.fx_browser_fx_chain_id; });

selector<mixer::channel_id> const select_focused_fx_chain([](state const& st) {
    return st.focused_fx_chain_id;
});

selector<fx::module_id> const select_focused_fx_module([](state const& st) {
    return st.focused_fx_mod_id;
});

static auto
get_focused_fx_module_name(
    fx::modules_t const& fx_modules,
    fx::module_id const focused_fx_mod_id)
{
    static boxed_string s_empty_name;

    fx::module const* const fx_mod = fx_modules.find(focused_fx_mod_id);
    return fx_mod ? fx_mod->name : s_empty_name;
}

selector<boxed_string> const select_focused_fx_module_name(
    [get = shared_memo(&get_focused_fx_module_name)](state const& st) {
        return get(st.fx_state.modules, st.focused_fx_mod_id);
    });

selector<material_color> const select_focused_fx_module_color(
    [focused_fx_chain = std::optional<mixer::channel_id>{},
     color = cached_entity_data_ptr<material_color>{}](
        state const& st) mutable {
        if (focused_fx_chain && *focused_fx_chain == st.focused_fx_chain_id &&
            color) [[likely]]
        {
            return *color;
        }

        focused_fx_chain = st.focused_fx_chain_id;
        color = st.material_colors.cached(
            st.mixer_state.channels.at(st.focused_fx_chain_id).color);
        return color ? *color : material_color::pink;
    });

selector<bool> const select_can_show_prev_fx_module([](state const& st) {
    auto fx_chain = st.mixer_state.fx_chains.find(st.focused_fx_chain_id);
    return fx_chain && !fx_chain->empty() &&
           fx_chain->front() != st.focused_fx_mod_id;
});

selector<bool> const select_can_show_next_fx_module([](state const& st) {
    auto fx_chain = st.mixer_state.fx_chains.find(st.focused_fx_chain_id);
    return fx_chain && !fx_chain->empty() &&
           fx_chain->back() != st.focused_fx_mod_id;
});

selector<startup_session> const select_startup_session([](state const& st) {
    return st.startup_session;
});

selector<box<std::filesystem::path>> const
    select_current_session([](state const& st) { return st.current_session; });

selector<bool> const select_session_modified([](state const& st) {
    return st.session_modified;
});

} // namespace piejam::runtime::selectors
