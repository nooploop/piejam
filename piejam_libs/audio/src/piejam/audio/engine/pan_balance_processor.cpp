// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/pan_balance_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/pan.h>
#include <piejam/audio/types.h>

#include <array>

namespace piejam::audio::engine
{

namespace
{

template <bus_type C>
class pan_balance_processor final : public named_processor
{
public:
    pan_balance_processor(std::string_view const& name)
        : named_processor(name)
    {
    }

    auto type_name() const -> std::string_view override
    {
        return C == bus_type::mono ? "pan" : "balance";
    }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override
    {
        static std::array s_ports{event_port{
                std::in_place_type<float>,
                C == bus_type::mono ? "pan" : "balance"}};
        return s_ports;
    }

    auto event_outputs() const -> event_ports override
    {
        static std::array s_ports{
                event_port{std::in_place_type<float>, "gain L"},
                event_port{std::in_place_type<float>, "gain R"}};
        return s_ports;
    }

    static constexpr auto pan_balance_factors(float const pan_balance)
            -> stereo_gain
    {
        return C == bus_type::mono ? sinusoidal_constant_power_pan(pan_balance)
                                   : stereo_balance(pan_balance);
    }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        audio::engine::event_buffer<float> const& ev_buf_pan =
                ctx.event_inputs.get<float>(0);

        if (ev_buf_pan.empty())
            return;

        event_buffer<float>& ev_buf_left = ctx.event_outputs.get<float>(0);
        event_buffer<float>& ev_buf_right = ctx.event_outputs.get<float>(1);

        for (event<float> const& ev : ev_buf_pan)
        {
            auto const pbf = pan_balance_factors(ev.value());
            ev_buf_left.insert(ev.offset(), pbf.left);
            ev_buf_right.insert(ev.offset(), pbf.right);
        }
    }
};

} // namespace

auto
make_pan_processor(std::string_view const& name)
        -> std::unique_ptr<audio::engine::processor>
{
    return std::make_unique<pan_balance_processor<audio::bus_type::mono>>(name);
}

auto
make_stereo_balance_processor(std::string_view const& name)
        -> std::unique_ptr<audio::engine::processor>
{
    return std::make_unique<pan_balance_processor<audio::bus_type::stereo>>(
            name);
}

} // namespace piejam::audio::engine
