// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxScope.h>

#include <piejam/app/gui/model/FxStream.h>
#include <piejam/gui/model/ScopeLinesGenerator.h>
#include <piejam/runtime/fx/scope.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::app::gui::model
{

struct FxScope::Impl
{
    runtime::fx::module_id fx_mod_id;

    piejam::gui::model::ScopeLinesGenerator accumulatorA;
    piejam::gui::model::ScopeLinesGenerator accumulatorB;

    std::unique_ptr<piejam::gui::model::AudioStreamProvider> streamA;
    std::unique_ptr<piejam::gui::model::AudioStreamProvider> streamB;
};

FxScope::FxScope(
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

    m_impl->streamA = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connect(*m_impl->streamA);

    m_impl->streamA->setListener(&m_impl->accumulatorA);
    m_impl->accumulatorA.setActive(activeA());
    m_impl->accumulatorA.setChannel(channelA());

    QObject::connect(
            &m_impl->accumulatorA,
            &piejam::gui::model::ScopeLinesGenerator::generated,
            this,
            [this](piejam::gui::model::ScopeLines const& addedLines) {
                dataA()->get().shift_push_back(addedLines);
                dataA()->update();
            });

    m_impl->streamB = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connect(*m_impl->streamB);

    m_impl->streamB->setListener(&m_impl->accumulatorB);
    m_impl->accumulatorB.setActive(activeB());
    m_impl->accumulatorB.setChannel(channelB());

    QObject::connect(
            &m_impl->accumulatorB,
            &piejam::gui::model::ScopeLinesGenerator::generated,
            this,
            [this](piejam::gui::model::ScopeLines const& addedLines) {
                dataB()->get().shift_push_back(addedLines);
                dataB()->update();
            });

    QObject::connect(this, &FxScope::activeAChanged, this, [this]() {
        m_impl->accumulatorA.setActive(activeA());
    });

    QObject::connect(this, &FxScope::channelAChanged, this, [this]() {
        m_impl->accumulatorA.setChannel(channelA());
    });

    QObject::connect(this, &FxScope::activeBChanged, this, [this]() {
        m_impl->accumulatorB.setActive(activeB());
    });

    QObject::connect(this, &FxScope::channelBChanged, this, [this]() {
        m_impl->accumulatorB.setChannel(channelB());
    });

    QObject::connect(this, &FxScope::samplesPerLineChanged, this, [this]() {
        m_impl->accumulatorA.setSamplesPerLine(samplesPerLine());
        m_impl->accumulatorB.setSamplesPerLine(samplesPerLine());
    });
}

FxScope::~FxScope() = default;

void
FxScope::onSubscribe()
{
    requestUpdates(std::chrono::milliseconds{16}, [this]() {
        m_impl->streamA->requestUpdate();
    });
}

} // namespace piejam::app::gui::model
