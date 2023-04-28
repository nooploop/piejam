// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/identity_processor.h>

#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>

#include <boost/assert.hpp>

namespace piejam::audio::engine
{

namespace
{

class identity_processor final : public named_processor
{
public:
    identity_processor(std::string_view const name)
        : named_processor(name)
    {
    }

    [[nodiscard]] auto type_name() const noexcept -> std::string_view override
    {
        return "audio_identity";
    }

    [[nodiscard]] auto num_inputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    [[nodiscard]] auto num_outputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    [[nodiscard]] auto event_inputs() const noexcept -> event_ports override
    {
        return {};
    }

    [[nodiscard]] auto event_outputs() const noexcept -> event_ports override
    {
        return {};
    }

    void process(process_context const& ctx) override
    {
        BOOST_ASSERT_MSG(
                false,
                "Should be removed from graph before executing.");

        verify_process_context(*this, ctx);

        ctx.results[0] = ctx.inputs[0];
    }
};

} // namespace

auto
make_identity_processor(std::string_view const name)
        -> std::unique_ptr<processor>
{
    return std::make_unique<identity_processor>(name);
}

auto
is_identity_processor(processor const& p) -> bool
{
    return typeid(p) == typeid(identity_processor);
}

} // namespace piejam::audio::engine
