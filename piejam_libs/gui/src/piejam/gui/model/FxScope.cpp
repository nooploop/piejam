// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxScope.h>

#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/ScopeLinesGenerator.h>

#include <piejam/audio/types.h>
#include <piejam/runtime/modules/scope/scope_module.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::gui::model
{

struct FxScope::Impl
{
    runtime::fx::module_id fx_mod_id;

    ScopeLinesGenerator accumulatorA;
    ScopeLinesGenerator accumulatorB;

    std::unique_ptr<AudioStreamProvider> streamA;
    std::unique_ptr<AudioStreamProvider> streamB;
};

FxScope::FxScope(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
    , m_busType{toBusType(observe_once(
              runtime::selectors::make_fx_module_bus_type_selector(fx_mod_id)))}
{
    auto const streams = observe_once(
            runtime::selectors::make_fx_module_streams_selector(fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::modules::scope::stream_key::input);
    auto const streamId = streams->at(streamKey);
    FxStreamKeyId fxStreamKeyId{.key = streamKey, .id = streamId};

    m_impl->streamA = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connectSubscribableChild(*m_impl->streamA);

    m_impl->streamA->setListener(&m_impl->accumulatorA);
    m_impl->accumulatorA.setStreamType(m_busType);
    m_impl->accumulatorA.setActive(activeA());
    m_impl->accumulatorA.setStereoChannel(channelA());

    QObject::connect(
            &m_impl->accumulatorA,
            &ScopeLinesGenerator::generated,
            this,
            [this](ScopeLines const& addedLines) {
                dataA()->get().shift_push_back(addedLines);
                dataA()->update();
            });

    if (m_busType == BusType::Stereo)
    {
        m_impl->streamB = std::make_unique<FxStream>(
                dispatch(),
                this->state_change_subscriber(),
                fxStreamKeyId);

        connectSubscribableChild(*m_impl->streamB);

        m_impl->streamB->setListener(&m_impl->accumulatorB);
        m_impl->accumulatorB.setStreamType(m_busType);
        m_impl->accumulatorB.setActive(activeB());
        m_impl->accumulatorB.setStereoChannel(channelB());

        QObject::connect(
                &m_impl->accumulatorB,
                &ScopeLinesGenerator::generated,
                this,
                [this](ScopeLines const& addedLines) {
                    dataB()->get().shift_push_back(addedLines);
                    dataB()->update();
                });
    }
}

FxScope::~FxScope() = default;

void
FxScope::onSubscribe()
{
    requestUpdates(std::chrono::milliseconds{16}, [this]() {
        m_impl->streamA->requestUpdate();
        // no need to update streamB, since it points to same stream as streamA
    });
}

void
FxScope::setSamplesPerLine(int const x)
{
    if (m_samplesPerLine != x)
    {
        m_samplesPerLine = x;
        m_impl->accumulatorA.setSamplesPerLine(x);
        m_impl->accumulatorB.setSamplesPerLine(x);
        emit samplesPerLineChanged();
    }
}

void
FxScope::changeActiveA(bool const active)
{
    if (m_activeA != active)
    {
        m_activeA = active;
        m_impl->accumulatorA.setActive(active);
        emit activeAChanged();

        clear();
    }
}

void
FxScope::changeChannelA(piejam::gui::model::StereoChannel const x)
{
    if (m_channelA != x)
    {
        m_channelA = x;
        m_impl->accumulatorA.setStereoChannel(x);
        emit channelAChanged();
    }
}

void
FxScope::changeActiveB(bool const active)
{
    if (m_activeB != active)
    {
        m_activeB = active;
        m_impl->accumulatorB.setActive(active);
        emit activeBChanged();

        clear();
    }
}

void
FxScope::changeChannelB(piejam::gui::model::StereoChannel const x)
{
    if (m_channelB != x)
    {
        m_channelB = x;
        m_impl->accumulatorB.setStereoChannel(x);
        emit channelBChanged();
    }
}

} // namespace piejam::gui::model
