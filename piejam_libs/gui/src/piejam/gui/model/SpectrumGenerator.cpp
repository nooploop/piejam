// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/SpectrumGenerator.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/math.h>
#include <piejam/numeric/dft.h>
#include <piejam/numeric/window.h>
#include <piejam/range/iota.h>

#include <boost/assert.hpp>

#include <vector>

namespace piejam::gui::model
{

namespace
{

auto
dftForResolution(DFTResolution const resolution) -> numeric::dft&
{
    switch (resolution)
    {
        case DFTResolution::Medium:
        {
            static numeric::dft s_dft{4096};
            return s_dft;
        }

        case DFTResolution::High:
        {
            static numeric::dft s_dft{8192};
            return s_dft;
        }

        case DFTResolution::VeryHigh:
        {
            static numeric::dft s_dft{16384};
            return s_dft;
        }

        case DFTResolution::Low:
        default:
        {
            static numeric::dft s_dft{2048};
            return s_dft;
        }
    }
}

} // namespace

struct SpectrumGenerator::Impl
{
    explicit Impl(audio::sample_rate sample_rate, DFTResolution dftResolution)
        : m_dft{dftForResolution(dftResolution)}
    {
        float const binSize =
                sample_rate.as_float() / static_cast<float>(m_dft.size());
        for (std::size_t const i : range::iota(m_dft.output_size()))
        {
            m_dataPoints[i].frequency_Hz = static_cast<float>(i) * binSize;
        }
    }

    static constexpr auto
    envelope(float const prev, float const in) noexcept -> float
    {
        BOOST_ASSERT(in >= 0.f);
        return in > prev ? in : in + 0.85f * (prev - in);
    }

    auto process(std::span<float const> samples) -> SpectrumDataPoints
    {
        std::transform(
                samples.begin(),
                samples.end(),
                m_window.begin(),
                m_dft.input_buffer().begin(),
                std::multiplies<>{});

        auto const spectrum = m_dft.process();

        BOOST_ASSERT(spectrum.size() == m_dft.output_size());
        BOOST_ASSERT(m_dataPoints.size() == m_dft.output_size());
        for (std::size_t const i : range::iota(m_dft.output_size()))
        {
            m_dataPoints[i].level = envelope(
                    m_dataPoints[i].level,
                    std::abs(spectrum[i]) * (2.f / m_dft.size()));
            m_dataPoints[i].level_dB = math::to_dB(m_dataPoints[i].level);
        }

        return m_dataPoints;
    }

    numeric::dft& m_dft;
    std::vector<float> m_window{algorithm::transform_to_vector(
            range::iota(m_dft.size()),
            std::bind_front(&numeric::window::hann, m_dft.size()))};
    std::vector<SpectrumDataPoint> m_dataPoints{
            std::vector<SpectrumDataPoint>(m_dft.output_size())};
};

SpectrumGenerator::SpectrumGenerator(
        audio::sample_rate sample_rate,
        DFTResolution dftResolution)
    : m_impl{make_pimpl<Impl>(sample_rate, dftResolution)}
    , m_dftPrepareBuffer(m_impl->m_dft.size())
{
}

auto
SpectrumGenerator::process() -> SpectrumDataPoints
{
    return m_impl->process(m_dftPrepareBuffer);
}

} // namespace piejam::gui::model
