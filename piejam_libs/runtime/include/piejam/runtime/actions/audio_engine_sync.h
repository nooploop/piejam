// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/audio_engine_action.h>
#include <piejam/runtime/actions/recorder_action.h>
#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <piejam/entity_id.h>

#include <boost/container/flat_map.hpp>

#include <tuple>

namespace piejam::runtime::actions
{

struct request_audio_engine_sync final
    : ui::cloneable_action<request_audio_engine_sync, action>
    , visitable_audio_engine_action<request_audio_engine_sync>
{
};

struct audio_engine_sync_update final
    : ui::cloneable_action<audio_engine_sync_update, reducible_action>
    , visitable_recorder_action<audio_engine_sync_update>
{
    template <class Parameter>
    using id_value_map_t = boost::container::flat_map<
            parameter::id_t<Parameter>,
            parameter_value_type_t<Parameter>>;

    using parameter_values_t = boost::mp11::mp_rename<
            boost::mp11::mp_transform<id_value_map_t, parameter_ids_t>,
            std::tuple>;

    parameter_values_t values;
    boost::container::flat_map<audio_stream_id, audio_stream_buffer> streams;

    template <class P>
    void push_back(
            parameter::id_t<P> const id,
            parameter_value_type_t<P> const value)
    {
        std::get<id_value_map_t<P>>(values).emplace(id, value);
    }

    [[nodiscard]]
    auto empty() const noexcept -> bool;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
