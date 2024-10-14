// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/filter/gui/FxFilter.h>

#include <piejam/fx_modules/filter/filter_internal_id.h>
#include <piejam/fx_modules/filter/filter_module.h>

#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/SpectrumGenerator.h>
#include <piejam/gui/model/SpectrumSlot.h>
#include <piejam/renew.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::filter::gui
{

using namespace piejam::gui::model;

struct FxFilter::Impl
{
    static constexpr audio::sample_rate default_sample_rate{48000u};

    Impl(BusType busType)
        : busType{busType}
    {
    }

    BusType busType;
    audio::sample_rate sample_rate{default_sample_rate};

    SpectrumGenerator spectrumInGenerator{sample_rate};
    SpectrumGenerator spectrumOutGenerator{sample_rate};

    SpectrumSlot spectrumIn;
    SpectrumSlot spectrumOut;

    std::unique_ptr<EnumParameter> filterTypeParam;
    std::unique_ptr<FloatParameter> cutoffParam;
    std::unique_ptr<FloatParameter> resonanceParam;
    std::unique_ptr<FxStream> inOutStream;

    void updateSampleRate(audio::sample_rate sr)
    {
        if (sr.valid() && sample_rate != sr)
        {
            sample_rate = sr;
            renew(spectrumInGenerator, sr);
            renew(spectrumOutGenerator, sr);
        }
    }
};

FxFilter::FxFilter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : FxModule{store_dispatch, state_change_subscriber, fx_mod_id}
    , m_impl{make_pimpl<Impl>(toBusType(
              observe_once(runtime::selectors::make_fx_module_bus_type_selector(
                      fx_mod_id))))}
{
    auto const& parameters = this->parameters();

    makeParameter(
            m_impl->filterTypeParam,
            parameters.at(to_underlying(parameter_key::type)));

    makeParameter(
            m_impl->cutoffParam,
            parameters.at(to_underlying(parameter_key::cutoff)));

    makeParameter(
            m_impl->resonanceParam,
            parameters.at(to_underlying(parameter_key::resonance)));

    makeStream(
            to_underlying(stream_key::in_out),
            m_impl->inOutStream,
            streams());

    if (m_impl->busType == BusType::Mono)
    {
        QObject::connect(
                m_impl->inOutStream.get(),
                &AudioStreamProvider::captured,
                this,
                [this](AudioStream captured) {
                    BOOST_ASSERT(captured.num_channels() == 2);

                    m_impl->spectrumIn.update(
                            m_impl->spectrumInGenerator.process(
                                    captured.channels_cast<2>().channels()[0]));
                    m_impl->spectrumOut.update(
                            m_impl->spectrumOutGenerator.process(
                                    captured.channels_cast<2>().channels()[1]));
                });
    }
    else
    {
        QObject::connect(
                m_impl->inOutStream.get(),
                &AudioStreamProvider::captured,
                this,
                [this](AudioStream captured) {
                    BOOST_ASSERT(captured.num_channels() == 4);

                    m_impl->spectrumIn.update(
                            m_impl->spectrumInGenerator.process(
                                    toMiddle(captured.channels_subview(0, 2)
                                                     .channels_cast<2>())));
                    m_impl->spectrumOut.update(
                            m_impl->spectrumOutGenerator.process(
                                    toMiddle(captured.channels_subview(2, 2)
                                                     .channels_cast<2>())));
                });
    }
}

void
FxFilter::onSubscribe()
{
    m_impl->updateSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);
}

auto
FxFilter::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

auto
FxFilter::spectrumIn() const noexcept -> SpectrumSlot*
{
    return &m_impl->spectrumIn;
}

auto
FxFilter::spectrumOut() const noexcept -> SpectrumSlot*
{
    return &m_impl->spectrumOut;
}

void
FxFilter::clear()
{
    m_impl->spectrumIn.clear();
    m_impl->spectrumOut.clear();
}

auto
FxFilter::filterType() const noexcept -> EnumParameter*
{
    return m_impl->filterTypeParam.get();
}

auto
FxFilter::cutoff() const noexcept -> FloatParameter*
{
    return m_impl->cutoffParam.get();
}

auto
FxFilter::resonance() const noexcept -> FloatParameter*
{
    return m_impl->resonanceParam.get();
}

} // namespace piejam::fx_modules::filter::gui
