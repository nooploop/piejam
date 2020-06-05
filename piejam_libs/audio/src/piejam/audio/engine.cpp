// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/audio/engine.h>

#include <piejam/algorithm/copy.h>
#include <piejam/algorithm/transform.h>
#include <piejam/audio/mixer.h>
#include <piejam/audio/period_sizes.h>
#include <piejam/range/table_view.h>

#include <boost/container/static_vector.hpp>

#include <algorithm>

namespace piejam::audio
{

static constexpr auto
level_meter_decay_time(unsigned const samplerate) -> std::size_t
{
    return samplerate / 10;
}

static constexpr auto
level_meter_rms_window_size(unsigned const samplerate) -> std::size_t
{
    constexpr double ws_in_ms = 400;
    return static_cast<std::size_t>((ws_in_ms / 1000.) * samplerate);
}

engine::engine(unsigned const samplerate, std::size_t const num_inputs)
    : m_mixer_state(num_inputs)
    , m_in_level_meters(
              num_inputs,
              {level_meter_decay_time(samplerate),
               level_meter_rms_window_size(samplerate)})
    , m_out_level_meter(
              {level_meter_decay_time(samplerate),
               level_meter_rms_window_size(samplerate)})
    , m_in_gain_smoothers(num_inputs)
{
}

void
engine::set_input_channel_enabled(std::size_t const index, bool const enabled)
{
    assert(index < m_mixer_state.inputs.size());
    m_mixer_state.inputs[index].enabled.store(
            enabled,
            std::memory_order_relaxed);
}

void
engine::set_input_channel_gain(std::size_t const index, float const gain)
{
    assert(index < m_mixer_state.inputs.size());
    m_mixer_state.inputs[index].gain.store(gain, std::memory_order_relaxed);
}

void
engine::set_output_channel_gain(float const gain)
{
    m_mixer_state.output.gain.store(gain, std::memory_order_relaxed);
}

auto
engine::get_input_level(std::size_t const index) const noexcept -> float
{
    assert(index < m_mixer_state.inputs.size());
    return m_mixer_state.inputs[index].level.load(std::memory_order_relaxed);
}

auto
engine::get_output_level() const noexcept -> float
{
    return m_mixer_state.output.level.load(std::memory_order_relaxed);
}

template <class InputIterator, class OutputIterator>
static void
apply_gain(
        smoother<>& gain_smoother,
        float const gain,
        std::size_t const smoother_advance,
        InputIterator&& in_begin,
        InputIterator&& in_end,
        OutputIterator&& out)
{
    constexpr std::size_t gain_smoothing_period{256};

    gain_smoother.set(gain, gain_smoothing_period);

    if (gain_smoother.is_running())
    {
        std::transform(
                std::forward<InputIterator>(in_begin),
                std::forward<InputIterator>(in_end),
                gain_smoother,
                std::forward<OutputIterator>(out),
                std::multiplies<float>{});

        gain_smoother.advance(smoother_advance);
    }
    else if (gain_smoother.current() == 1.f)
    {
        std::copy(
                std::forward<InputIterator>(in_begin),
                std::forward<InputIterator>(in_end),
                std::forward<OutputIterator>(out));
    }
    else
    {
        std::transform(
                std::forward<InputIterator>(in_begin),
                std::forward<InputIterator>(in_end),
                std::forward<OutputIterator>(out),
                [gain](float x) { return x * gain; });
    }
}

void
engine::operator()(
        range::table_view<float const> const& ins,
        range::table_view<float> const& outs) noexcept
{
    using gain_buffer_t =
            boost::container::static_vector<float, max_period_size>;

    std::size_t const num_out_channels = outs.major_size();
    if (num_out_channels > 0)
        std::fill(outs[0].begin(), outs[0].end(), 0);

    std::size_t const num_in_channels = ins.major_size();
    if (num_in_channels > 0)
    {
        for (std::size_t ch = 0; ch < num_in_channels; ++ch)
        {
            mixer_channel& in_channel = m_mixer_state.inputs[ch];
            if (in_channel.enabled)
            {
                gain_buffer_t gain_buffer;

                apply_gain(
                        m_in_gain_smoothers[ch],
                        in_channel.gain,
                        ins[ch].size(),
                        ins[ch].begin(),
                        ins[ch].end(),
                        std::back_inserter(gain_buffer));

                // calculate level
                algorithm::copy(
                        gain_buffer,
                        std::back_inserter(m_in_level_meters[ch]));

                in_channel.level.store(
                        m_in_level_meters[ch].get(),
                        std::memory_order_relaxed);

                // mix
                if (num_out_channels > 0)
                {
                    std::transform(
                            gain_buffer.begin(),
                            gain_buffer.end(),
                            outs[0].begin(),
                            outs[0].begin(),
                            std::plus<float>{});
                }
            }
            else
            {
                m_in_level_meters[ch].clear();
                in_channel.level.store(0.f, std::memory_order_relaxed);
            }
        }
    }

    // apply output gain
    if (num_out_channels > 0)
    {
        apply_gain(
                m_out_gain_smoother,
                m_mixer_state.output.gain,
                outs[0].size(),
                outs[0].begin(),
                outs[0].end(),
                outs[0].begin());

        if (num_out_channels > 1)
            algorithm::copy(outs[0], outs[1].begin());

        algorithm::copy(outs[0], std::back_inserter(m_out_level_meter));
        m_mixer_state.output.level.store(
                m_out_level_meter.get(),
                std::memory_order_relaxed);
    }
}

} // namespace piejam::audio
