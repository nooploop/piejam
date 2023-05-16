// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxScope.h>

#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/WaveformDataGenerator.h>

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

    WaveformDataGenerator generatorA;
    WaveformDataGenerator generatorB;

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

    m_impl->streamA->setListener(&m_impl->generatorA);
    m_impl->generatorA.setStreamType(m_busType);
    m_impl->generatorA.setActive(activeA());
    m_impl->generatorA.setStereoChannel(channelA());

    QObject::connect(
            &m_impl->generatorA,
            &WaveformDataGenerator::generated,
            this,
            [this](WaveformData const& addedLines) {
                waveformDataA()->get().shift_push_back(addedLines);
                waveformDataA()->update();
            });

    if (m_busType == BusType::Stereo)
    {
        m_impl->streamB = std::make_unique<FxStream>(
                dispatch(),
                this->state_change_subscriber(),
                fxStreamKeyId);

        connectSubscribableChild(*m_impl->streamB);

        m_impl->streamB->setListener(&m_impl->generatorB);
        m_impl->generatorB.setStreamType(m_busType);
        m_impl->generatorB.setActive(activeB());
        m_impl->generatorB.setStereoChannel(channelB());

        QObject::connect(
                &m_impl->generatorB,
                &WaveformDataGenerator::generated,
                this,
                [this](WaveformData const& addedLines) {
                    waveformDataB()->get().shift_push_back(addedLines);
                    waveformDataB()->update();
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
        m_impl->generatorA.setSamplesPerLine(x);
        m_impl->generatorB.setSamplesPerLine(x);
        emit samplesPerLineChanged();
    }
}

void
FxScope::changeActiveA(bool const active)
{
    if (m_activeA != active)
    {
        m_activeA = active;
        m_impl->generatorA.setActive(active);
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
        m_impl->generatorA.setStereoChannel(x);
        emit channelAChanged();

        clear();
    }
}

void
FxScope::changeActiveB(bool const active)
{
    if (m_activeB != active)
    {
        m_activeB = active;
        m_impl->generatorB.setActive(active);
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
        m_impl->generatorB.setStereoChannel(x);
        emit channelBChanged();

        clear();
    }
}

void
FxScope::clear()
{
    m_waveformDataA.get().clear();
    m_waveformDataB.get().clear();

    if (m_activeA)
    {
        m_waveformDataA.get().resize(m_viewSize);
    }

    if (m_activeB)
    {
        m_waveformDataB.get().resize(m_viewSize);
    }

    m_impl->generatorA.clear();
    m_impl->generatorB.clear();

    m_waveformDataA.update();
    m_waveformDataB.update();
}

} // namespace piejam::gui::model
