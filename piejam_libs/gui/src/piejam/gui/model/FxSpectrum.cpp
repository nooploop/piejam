// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxSpectrum.h>

#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/SpectrumDataGenerator.h>
#include <piejam/runtime/modules/spectrum/spectrum_module.h>
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

struct FxSpectrum::Impl
{
    Impl(BusType busType)
        : busType{busType}
        , dataGenerator{substreamConfigs(busType)}
    {
    }

    BusType busType;

    SpectrumDataGenerator dataGenerator;

    std::unique_ptr<AudioStreamProvider> stream;
};

FxSpectrum::FxSpectrum(
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
            to_underlying(runtime::modules::spectrum::stream_key::input);
    auto const streamId = streams.get().at(streamKey);
    FxStreamKeyId fxStreamKeyId{.key = streamKey, .id = streamId};

    m_impl->stream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connectSubscribableChild(*m_impl->stream);

    m_impl->dataGenerator.setActive(0, activeA());
    m_impl->dataGenerator.setChannel(0, channelA());

    if (m_impl->busType == BusType::Stereo)
    {
        m_impl->dataGenerator.setActive(1, activeB());
        m_impl->dataGenerator.setChannel(1, channelB());
    }

    QObject::connect(
            &m_impl->dataGenerator,
            &SpectrumDataGenerator::generated,
            this,
            [this](std::span<SpectrumDataPoints const> dataPoints) {
                dataA()->set(dataPoints[0]);

                if (m_impl->busType == BusType::Stereo)
                {
                    dataB()->set(dataPoints[1]);
                }
            });

    if (m_impl->busType == BusType::Stereo)
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->dataGenerator,
                [this](AudioStreamListener::Stream stream) {
                    auto const buf = duplicate_channels(stream);
                    m_impl->dataGenerator.update(buf.view());
                });
    }
    else
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->dataGenerator,
                &SpectrumDataGenerator::update);
    }
}

FxSpectrum::~FxSpectrum() = default;

auto
FxSpectrum::busType() const noexcept -> BusType
{
    return m_impl->busType;
}

void
FxSpectrum::changeActiveA(bool const active)
{
    if (m_activeA != active)
    {
        m_activeA = active;
        m_impl->dataGenerator.setActive(0, activeA());
        emit activeAChanged();
    }
}

void
FxSpectrum::changeChannelA(piejam::gui::model::StereoChannel const x)
{
    if (m_channelA != x)
    {
        m_channelA = x;
        m_impl->dataGenerator.setChannel(0, channelA());
        emit channelAChanged();
    }
}

void
FxSpectrum::changeActiveB(bool const active)
{
    if (m_activeB != active)
    {
        m_activeB = active;
        m_impl->dataGenerator.setActive(1, activeB());
        emit activeBChanged();
    }
}

void
FxSpectrum::changeChannelB(piejam::gui::model::StereoChannel const x)
{
    if (m_channelB != x)
    {
        m_channelB = x;
        m_impl->dataGenerator.setChannel(1, channelB());
        emit channelBChanged();
    }
}

void
FxSpectrum::onSubscribe()
{
    m_impl->dataGenerator.setSampleRate(
            observe_once(runtime::selectors::select_sample_rate).second);
}

} // namespace piejam::gui::model
