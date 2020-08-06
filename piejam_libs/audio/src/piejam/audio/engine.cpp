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
#include <piejam/audio/pan.h>
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

engine::engine(
        unsigned const samplerate,
        std::vector<std::size_t> const& input_bus_config,
        pair<std::size_t> const& output_config)
    : m_mixer_state(
              input_bus_config.size(),
              level_meter_decay_time(samplerate),
              level_meter_rms_window_size(samplerate))
{
    std::size_t input_bus_index{};
    for (std::size_t in_channel : input_bus_config)
        m_mixer_state.inputs[input_bus_index++].device_channel = in_channel;
    m_mixer_state.output.device_channels = output_config;
}

void
engine::set_input_channel_gain(std::size_t const index, float const gain)
{
    assert(index < m_mixer_state.inputs.size());
    m_mixer_state.inputs[index].gain.store(gain, std::memory_order_relaxed);
}

void
engine::set_input_channel_pan(std::size_t const index, float const pan)
{
    assert(index < m_mixer_state.inputs.size());
    m_mixer_state.inputs[index].pan.store(pan, std::memory_order_relaxed);
}

void
engine::set_output_channel_gain(float const gain)
{
    m_mixer_state.output.gain.store(gain, std::memory_order_relaxed);
}

void
engine::set_output_channel_balance(float const balance)
{
    m_mixer_state.output.balance.store(balance, std::memory_order_relaxed);
}

auto
engine::get_input_level(std::size_t const index) const noexcept
        -> mixer::stereo_level
{
    assert(index < m_mixer_state.inputs.size());
    auto const& in_ch = m_mixer_state.inputs[index];
    return mixer::stereo_level{
            in_ch.level.left.load(std::memory_order_relaxed),
            in_ch.level.right.load(std::memory_order_relaxed)};
}

auto
engine::get_output_level() const noexcept -> mixer::stereo_level
{
    return mixer::stereo_level{
            m_mixer_state.output.level.left.load(std::memory_order_relaxed),
            m_mixer_state.output.level.right.load(std::memory_order_relaxed)};
}

template <class InputIterator, class OutputIterator>
static void
apply_gain(
        smoother<>& gain_smoother,
        float const gain,
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
    }
    else if (gain_smoother.current() == 1.f)
    {
        std::copy(
                std::forward<InputIterator>(in_begin),
                std::forward<InputIterator>(in_end),
                std::forward<OutputIterator>(out));
    }
    else if (gain_smoother.current() == 0.f)
    {
        std::fill_n(
                std::forward<OutputIterator>(out),
                std::distance(
                        std::forward<InputIterator>(in_begin),
                        std::forward<InputIterator>(in_end)),
                0.f);
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

template <class ChannelBuffer, class OutputIterator>
static void
apply_gain(
        smoother<>& gain_smoother,
        float const gain,
        ChannelBuffer const& buf,
        OutputIterator out)
{
    apply_gain(gain_smoother, gain, buf.begin(), buf.end(), out);
    gain_smoother.advance(buf.size());
}

template <class ChannelBuffer>
static void
calculate_level(
        ChannelBuffer const& buf,
        level_meter& lm,
        std::atomic<float>& level)
{
    algorithm::copy(buf, std::back_inserter(lm));
    level.store(lm.get(), std::memory_order_relaxed);
}

static void
reset_level(level_meter& lm, std::atomic<float>& level)
{
    lm.clear();
    level.store(0.f, std::memory_order_relaxed);
}

template <class ChannelBuffer, class InOutIterator>
static void
mix(ChannelBuffer const& buf, InOutIterator dst)
{
    std::transform(buf.begin(), buf.end(), dst, dst, std::plus<float>{});
}

void
engine::operator()(
        range::table_view<float const> const& ins,
        range::table_view<float> const& outs) noexcept
{
    using audio_buffer_t =
            boost::container::static_vector<float, max_period_size>;
    using stereo_audio_buffer_t = pair<audio_buffer_t>;

    // clear output
    for (auto ch : outs)
        std::fill(ch.begin(), ch.end(), 0.f);

    std::size_t const num_in_channels = ins.major_size();
    std::size_t const num_out_channels = outs.major_size();

    std::size_t const num_in_busses = m_mixer_state.inputs.size();

    stereo_audio_buffer_t mix_buffer(audio_buffer_t(outs.minor_size(), 0.f));

    for (std::size_t bus = 0; bus < num_in_busses; ++bus)
    {
        input_mixer_channel& in_channel = m_mixer_state.inputs[bus];
        if (in_channel.device_channel < num_in_channels)
        {
            stereo_audio_buffer_t gain_buffer;

            auto in_pan = sinusoidal_constant_power_pan(
                    in_channel.pan.load(std::memory_order_relaxed));

            std::size_t ch = in_channel.device_channel;

            // left channel
            {
                apply_gain(
                        in_channel.gain_smoother.left,
                        in_channel.gain * in_pan.left,
                        ins[ch],
                        std::back_inserter(gain_buffer.left));

                calculate_level(
                        gain_buffer.left,
                        in_channel.stereo_level_meter->left,
                        in_channel.level.left);

                mix(gain_buffer.left, mix_buffer.left.begin());
            }

            // right channel
            {
                apply_gain(
                        in_channel.gain_smoother.right,
                        in_channel.gain * in_pan.right,
                        ins[ch],
                        std::back_inserter(gain_buffer.right));

                calculate_level(
                        gain_buffer.right,
                        in_channel.stereo_level_meter->right,
                        in_channel.level.right);

                mix(gain_buffer.right, mix_buffer.right.begin());
            }
        }
        else
        {
            reset_level(
                    in_channel.stereo_level_meter->left,
                    in_channel.level.left);
            reset_level(
                    in_channel.stereo_level_meter->right,
                    in_channel.level.right);
        }
    }

    auto pow3 = [](auto x) { return x * x * x; };

    auto& out_ch = m_mixer_state.output;

    // left channel
    {
        float const balance =
                out_ch.balance <= 0.f ? 1.f : pow3(1 - out_ch.balance);
        apply_gain(
                out_ch.gain_smoother.left,
                out_ch.gain * balance,
                mix_buffer.left,
                mix_buffer.left.begin());

        calculate_level(
                mix_buffer.left,
                out_ch.stereo_level_meter->left,
                out_ch.level.left);
    }

    auto const left_channel = out_ch.device_channels.left;
    if (num_out_channels > left_channel)
    {
        algorithm::copy(mix_buffer.left, outs[left_channel].begin());
    }

    // right channel
    {
        float const balance =
                out_ch.balance >= 0.f ? 1.f : pow3(1 + out_ch.balance);
        apply_gain(
                out_ch.gain_smoother.right,
                out_ch.gain * balance,
                mix_buffer.right,
                mix_buffer.right.begin());

        calculate_level(
                mix_buffer.right,
                out_ch.stereo_level_meter->right,
                out_ch.level.right);
    }

    auto const right_channel = out_ch.device_channels.right;
    if (num_out_channels > right_channel)
    {
        algorithm::copy(mix_buffer.right, outs[right_channel].begin());
    }
}

} // namespace piejam::audio
