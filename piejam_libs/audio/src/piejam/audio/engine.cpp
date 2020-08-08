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
#include <piejam/range/span.h>
#include <piejam/range/table_view.h>

#include <boost/container/static_vector.hpp>

#include <algorithm>

namespace piejam::audio
{

using audio_buffer_t = boost::container::static_vector<float, max_period_size>;
using stereo_audio_buffer_t = pair<audio_buffer_t>;

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
        std::vector<std::pair<bus_type, channel_index_pair>> const&
                input_config,
        std::vector<channel_index_pair> const& output_config)
    : m_mixer_state(
              input_config.size(),
              output_config.size(),
              level_meter_decay_time(samplerate),
              level_meter_rms_window_size(samplerate))
{
    std::size_t input_bus_index{};
    for (auto const& in_conf : input_config)
    {
        auto& in_ch = m_mixer_state.inputs[input_bus_index++];
        in_ch.type = in_conf.first;
        in_ch.device_channels = in_conf.second;
    }

    std::size_t output_bus_index{};
    for (auto const& out_conf : output_config)
    {
        auto& out_ch = m_mixer_state.outputs[output_bus_index++];
        out_ch.type = bus_type::stereo;
        out_ch.device_channels = out_conf;
    }
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
    m_mixer_state.inputs[index].pan_balance.store(
            pan,
            std::memory_order_relaxed);
}

void
engine::set_output_channel_gain(std::size_t const index, float const gain)
{
    assert(index < m_mixer_state.outputs.size());
    m_mixer_state.outputs[index].gain.store(gain, std::memory_order_relaxed);
}

void
engine::set_output_channel_balance(std::size_t const index, float const balance)
{
    assert(index < m_mixer_state.outputs.size());
    m_mixer_state.outputs[index].pan_balance.store(
            balance,
            std::memory_order_relaxed);
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
engine::get_output_level(std::size_t const index) const noexcept
        -> mixer::stereo_level
{
    assert(index < m_mixer_state.outputs.size());
    auto const& out_ch = m_mixer_state.outputs[index];
    return mixer::stereo_level{
            out_ch.level.left.load(std::memory_order_relaxed),
            out_ch.level.right.load(std::memory_order_relaxed)};
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

template <class MixerChannel>
static void
apply_gain_and_calculate_level(
        MixerChannel& mc,
        range::span<float const> source_left_buffer,
        range::span<float const> source_right_buffer,
        stereo_gain const gain,
        stereo_audio_buffer_t& gain_buffer)
{
    if (!source_left_buffer.empty())
    {
        apply_gain(
                mc.gain_smoother.left,
                mc.gain * gain.left,
                source_left_buffer,
                std::back_inserter(gain_buffer.left));

        calculate_level(
                gain_buffer.left,
                mc.stereo_level_meter->left,
                mc.level.left);
    }
    else
    {
        reset_level(mc.stereo_level_meter->left, mc.level.left);
    }

    if (!source_right_buffer.empty())
    {
        apply_gain(
                mc.gain_smoother.right,
                mc.gain * gain.right,
                source_right_buffer,
                std::back_inserter(gain_buffer.right));

        calculate_level(
                gain_buffer.right,
                mc.stereo_level_meter->right,
                mc.level.right);
    }
    else
    {
        reset_level(mc.stereo_level_meter->right, mc.level.right);
    }
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
    // clear output
    for (auto ch : outs)
        std::fill(ch.begin(), ch.end(), 0.f);

    std::size_t const num_in_channels = ins.major_size();
    std::size_t const num_out_channels = outs.major_size();

    std::size_t const num_in_busses = m_mixer_state.inputs.size();

    stereo_audio_buffer_t mix_buffer(audio_buffer_t(ins.minor_size(), 0.f));

    for (std::size_t bus = 0; bus < num_in_busses; ++bus)
    {
        auto& in_channel = m_mixer_state.inputs[bus];
        assert(in_channel.type == bus_type::mono);

        stereo_audio_buffer_t gain_buffer;

        float const in_pan_balance_pos =
                in_channel.pan_balance.load(std::memory_order_relaxed);
        auto const in_pan_balance =
                in_channel.type == bus_type::mono
                        ? sinusoidal_constant_power_pan(in_pan_balance_pos)
                        : stereo_balance(in_pan_balance_pos);

        range::span<float const> in_left_buffer;
        if (auto ch = in_channel.device_channels.left; ch < num_in_channels)
            in_left_buffer = {ins[ch].data(), ins[ch].size()};
        range::span<float const> in_right_buffer;
        if (auto ch = in_channel.device_channels.right; ch < num_in_channels)
            in_right_buffer = {ins[ch].data(), ins[ch].size()};

        apply_gain_and_calculate_level(
                in_channel,
                in_left_buffer,
                in_right_buffer,
                in_pan_balance,
                gain_buffer);

        mix(gain_buffer.left, mix_buffer.left.begin());
        mix(gain_buffer.right, mix_buffer.right.begin());
    }

    for (auto& out_ch : m_mixer_state.outputs)
    {
        stereo_audio_buffer_t gain_buffer;

        assert(out_ch.type == bus_type::stereo);
        auto const out_balance = stereo_balance(out_ch.pan_balance);

        apply_gain_and_calculate_level(
                out_ch,
                {mix_buffer.left.data(), mix_buffer.left.size()},
                {mix_buffer.right.data(), mix_buffer.right.size()},
                out_balance,
                gain_buffer);

        auto const left_channel = out_ch.device_channels.left;
        if (num_out_channels > left_channel)
        {
            algorithm::copy(gain_buffer.left, outs[left_channel].begin());
        }

        auto const right_channel = out_ch.device_channels.right;
        if (num_out_channels > right_channel)
        {
            algorithm::copy(gain_buffer.right, outs[right_channel].begin());
        }
    }
}

} // namespace piejam::audio
