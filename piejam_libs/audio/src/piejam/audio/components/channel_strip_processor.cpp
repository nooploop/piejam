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

#include <piejam/audio/components/channel_strip_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/pan.h>

#include <boost/assert.hpp>

namespace piejam::audio::components
{

namespace
{

enum class channel_strip_type
{
    mono,
    stereo
};

template <channel_strip_type C>
class channel_strip_processor final : public engine::processor
{
public:
    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }

    // event-ins:
    //   0: pan / balance
    //   1: volume
    auto num_event_inputs() const -> std::size_t override { return 2; }

    // event-outs:
    //   0: left channel amplify factor
    //   1: right channel amplify factor
    auto num_event_outputs() const -> std::size_t override { return 2; }

    void create_event_output_buffers(
            engine::event_output_buffer_factory const& f) const override
    {
        f.add<float>();
        f.add<float>();
    }

    static auto pan_balance_gain(float const pan_balance) -> stereo_gain
    {
        return C == channel_strip_type::mono
                       ? sinusoidal_constant_power_pan(pan_balance)
                       : stereo_balance(pan_balance);
    }

    void process_pan_balance_event(
            engine::event_buffer<float>& ev_buf_left,
            engine::event_buffer<float>& ev_buf_right,
            engine::event<float> const& ev)
    {
        auto pbg = pan_balance_gain(ev.value());
        ev_buf_left.insert(ev.offset(), pbg.left * m_last_volume);
        ev_buf_right.insert(ev.offset(), pbg.right * m_last_volume);
        m_last_pan_balance = ev.value();
    }

    void process_volume_event(
            engine::event_buffer<float>& ev_buf_left,
            engine::event_buffer<float>& ev_buf_right,
            engine::event<float> const& ev)
    {
        auto pbg = pan_balance_gain(m_last_pan_balance);
        ev_buf_left.insert(ev.offset(), pbg.left * ev.value());
        ev_buf_right.insert(ev.offset(), pbg.right * ev.value());
        m_last_volume = ev.value();
    }

    void process(engine::process_context const& ctx) override
    {
        engine::event_buffer<float> const* const ev_buf_pan =
                ctx.event_inputs.get<float>(0);
        BOOST_ASSERT(ev_buf_pan);

        engine::event_buffer<float> const* const ev_buf_vol =
                ctx.event_inputs.get<float>(1);
        BOOST_ASSERT(ev_buf_vol);

        if (ev_buf_pan->empty() && ev_buf_vol->empty())
            return;

        engine::event_buffer<float>& ev_buf_left =
                ctx.event_outputs.get<float>(0);
        engine::event_buffer<float>& ev_buf_right =
                ctx.event_outputs.get<float>(1);

        auto pan_it = ev_buf_pan->begin();
        auto const pan_last = ev_buf_pan->end();
        auto vol_it = ev_buf_vol->begin();
        auto const vol_last = ev_buf_vol->end();

        while (pan_it != pan_last || vol_it != vol_last)
        {
            if (pan_it != pan_last && vol_it != vol_last)
            {
                if (pan_it->offset() < vol_it->offset())
                {
                    process_pan_balance_event(
                            ev_buf_left,
                            ev_buf_right,
                            *pan_it++);
                }
                else if (pan_it->offset() > vol_it->offset())
                {
                    process_volume_event(ev_buf_left, ev_buf_right, *vol_it++);
                }
                else
                {
                    auto pbg = pan_balance_gain(pan_it->value());
                    ev_buf_left.insert(
                            pan_it->offset(),
                            pbg.left * vol_it->value());
                    ev_buf_right.insert(
                            pan_it->offset(),
                            pbg.right * vol_it->value());
                    m_last_pan_balance = pan_it->value();
                    ++pan_it;
                    m_last_volume = vol_it->value();
                    ++vol_it;
                }
            }
            else if (pan_it != pan_last)
            {
                process_pan_balance_event(ev_buf_left, ev_buf_right, *pan_it++);
            }
            else if (vol_it != vol_last)
            {
                process_volume_event(ev_buf_left, ev_buf_right, *vol_it++);
            }
        }
    }

private:
    float m_last_pan_balance{0.f};
    float m_last_volume{1.f};
};

} // namespace

auto
make_mono_channel_strip_processor() -> std::unique_ptr<engine::processor>
{
    return std::make_unique<
            channel_strip_processor<channel_strip_type::mono>>();
}

auto
make_stereo_channel_strip_processor() -> std::unique_ptr<engine::processor>
{
    return std::make_unique<
            channel_strip_processor<channel_strip_type::stereo>>();
}

} // namespace piejam::audio::components
