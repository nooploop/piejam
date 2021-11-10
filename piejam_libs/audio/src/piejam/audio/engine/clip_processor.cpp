// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/clip_processor.h>

#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>

namespace piejam::audio::engine
{

namespace
{

class clip_processor final : public named_processor
{
public:
    clip_processor(float min, float max, std::string_view const name)
        : named_processor(name)
        , m_min(min)
        , m_max(max)
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "clip";
    }

    auto num_inputs() const noexcept -> std::size_t override { return 1; }
    auto num_outputs() const noexcept -> std::size_t override { return 1; }

    auto event_inputs() const noexcept -> event_ports override { return {}; }
    auto event_outputs() const noexcept -> event_ports override { return {}; }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        ctx.results[0] =
                clamp(ctx.inputs[0].get(), m_min, m_max, ctx.outputs[0]);
    }

private:
    float m_min;
    float m_max;
};

} // namespace

auto
make_clip_processor(
        float const min,
        float const max,
        std::string_view const name) -> std::unique_ptr<processor>
{
    return std::make_unique<clip_processor>(min, max, name);
}

} // namespace piejam::audio::engine
