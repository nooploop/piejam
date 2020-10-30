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

#include <piejam/audio/engine/amplify_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/smoother.h>

#include <algorithm>
#include <array>

namespace piejam::audio::engine
{

namespace
{

class amplify_processor final : public named_processor
{
public:
    amplify_processor(std::string_view const& name)
        : named_processor(name)
    {
    }

    auto type_name() const -> std::string_view override { return "amplify"; }

    auto num_inputs() const -> std::size_t override { return 1; }
    auto num_outputs() const -> std::size_t override { return 1; }
    auto event_inputs() const -> event_ports override
    {
        static std::array s_ports{event_port{typeid(float), "gain"}};
        return s_ports;
    }
    auto event_outputs() const -> event_ports override { return {}; }

    void create_event_output_buffers(
            event_output_buffer_factory const&) const override
    {
    }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        event_buffer<float> const* const amp_ev_buf =
                ctx.event_inputs.get<float>(0);
        std::span<float const> const in_buf = ctx.inputs[0];
        std::span<float> const out_buf = ctx.outputs[0];
        std::span<float const>& res_buf = ctx.results[0];

        if (!amp_ev_buf || amp_ev_buf->empty())
        {
            if (m_factor.is_running())
            {
                if (!in_buf.empty())
                {
                    std::transform(
                            in_buf.begin(),
                            in_buf.end(),
                            m_factor.advance_iterator(),
                            out_buf.begin(),
                            std::multiplies<float>{});
                }
                else
                {
                    res_buf = in_buf;
                    m_factor.advance(ctx.buffer_size);
                }
            }
            else if (m_factor.current() == 1.f)
                res_buf = in_buf;
            else if (in_buf.empty() || m_factor.current() == 0.f)
                res_buf = {};
            else
                std::ranges::transform(
                        in_buf,
                        out_buf.begin(),
                        [f = m_factor.current()](float const sample) {
                            return f * sample;
                        });
        }
        else
        {
            constexpr float frames_to_smooth = 128;

            if (in_buf.empty())
            {
                res_buf = {};
                for (event<float> const& ev : *amp_ev_buf)
                    m_factor.set(ev.value(), 0);
            }
            else
            {
                std::size_t offset{};
                for (event<float> const& ev : *amp_ev_buf)
                {
                    BOOST_ASSERT(offset < in_buf.size());
                    std::transform(
                            std::next(in_buf.begin(), offset),
                            std::next(in_buf.begin(), ev.offset()),
                            m_factor.advance_iterator(),
                            std::next(out_buf.begin(), offset),
                            std::multiplies<float>{});
                    offset = ev.offset();
                    m_factor.set(ev.value(), frames_to_smooth);
                }

                std::transform(
                        std::next(in_buf.begin(), offset),
                        in_buf.end(),
                        m_factor.advance_iterator(),
                        std::next(out_buf.begin(), offset),
                        std::multiplies<float>{});
            }
        }
    }

private:
    smoother<> m_factor{1.f};
};

} // namespace

auto
make_amplify_processor(std::string_view const& name)
        -> std::unique_ptr<processor>
{
    return std::make_unique<amplify_processor>(name);
}

} // namespace piejam::audio::engine
