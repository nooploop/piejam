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

namespace
{

auto
substreamConfigs(BusType busType) -> std::span<BusType const>
{
    switch (busType)
    {
        case BusType::Mono:
        {
            static std::array const configs{BusType::Mono};
            return configs;
        }

        case BusType::Stereo:
        {
            static std::array const configs{BusType::Stereo, BusType::Stereo};
            return configs;
        }
    }
}

} // namespace

struct FxScope::Impl
{
    Impl(BusType busType)
        : busType{busType}
        , waveformGenerator{substreamConfigs(busType)}
    {
    }

    BusType busType;

    WaveformDataGenerator waveformGenerator;

    std::unique_ptr<AudioStreamProvider> stream;
};

FxScope::FxScope(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(toBusType(
              observe_once(runtime::selectors::make_fx_module_bus_type_selector(
                      fx_mod_id)))))
{
    auto const streams = observe_once(
            runtime::selectors::make_fx_module_streams_selector(fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::modules::scope::stream_key::input);
    auto const streamId = streams->at(streamKey);
    FxStreamKeyId fxStreamKeyId{.key = streamKey, .id = streamId};

    m_impl->stream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connectSubscribableChild(*m_impl->stream);

    m_impl->waveformGenerator.setActive(0, activeA());
    m_impl->waveformGenerator.setStereoChannel(0, channelA());

    if (m_busType == BusType::Stereo)
    {
        m_impl->waveformGenerator.setActive(1, activeB());
        m_impl->waveformGenerator.setStereoChannel(1, channelB());
    }

    QObject::connect(
            &m_impl->waveformGenerator,
            &WaveformDataGenerator::generated,
            this,
            [this](std::span<WaveformData const> addedLines) {
                waveformDataA()->get().shift_push_back(addedLines[0]);
                waveformDataA()->update();

                if (m_impl->busType == BusType::Stereo)
                {
                    waveformDataB()->get().shift_push_back(addedLines[1]);
                    waveformDataB()->update();
                }
            });

    if (m_impl->busType == BusType::Stereo)
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->waveformGenerator,
                [this](AudioStream const& stream) {
                    auto const buf = duplicate_channels(stream);
                    m_impl->waveformGenerator.update(buf.view());
                });
    }
    else
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->waveformGenerator,
                &WaveformDataGenerator::update);
    }
}

FxScope::~FxScope() = default;

auto
FxScope::busType() const noexcept -> BusType
{
    return m_impl->busType;
}

void
FxScope::setSamplesPerPixel(int const x)
{
    if (m_samplesPerPixel != x)
    {
        m_samplesPerPixel = x;
        m_impl->waveformGenerator.setSamplesPerPixel(x);
        emit samplesPerPixelChanged();
    }
}

void
FxScope::changeActiveA(bool const active)
{
    if (m_activeA != active)
    {
        m_activeA = active;
        m_impl->waveformGenerator.setActive(0, active);
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
        m_impl->waveformGenerator.setStereoChannel(0, x);
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
        m_impl->waveformGenerator.setActive(1, active);
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
        m_impl->waveformGenerator.setStereoChannel(1, x);
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

    m_impl->waveformGenerator.clear();

    m_waveformDataA.update();
    m_waveformDataB.update();
}

} // namespace piejam::gui::model
