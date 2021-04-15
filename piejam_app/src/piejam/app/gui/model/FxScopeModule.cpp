// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxScopeModule.h>

#include <piejam/app/gui/model/FxStream.h>
#include <piejam/runtime/fx/scope.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::app::gui::model
{

struct FxScopeModule::Impl
{
    runtime::fx::module_id fx_mod_id;

    std::unique_ptr<piejam::gui::model::AudioStreamProvider> scopeStream;
};

FxScopeModule::FxScopeModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
{
    auto const streams =
            observe_once(runtime::selectors::make_fx_module_streams_selector(
                    m_impl->fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::fx::scope_stream_key::left_right);
    auto const streamId = streams.get().at(streamKey);
    FxStreamKeyId fxStreamKeyId{.key = streamKey, .id = streamId};

    m_impl->scopeStream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connect(*m_impl->scopeStream);
}

FxScopeModule::~FxScopeModule() = default;

auto
FxScopeModule::scopeStream() noexcept
        -> piejam::gui::model::AudioStreamProvider*
{
    return m_impl->scopeStream.get();
}

} // namespace piejam::app::gui::model
