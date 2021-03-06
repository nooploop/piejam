// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/identity_processor.h>

#include <piejam/audio/engine/named_processor.h>

#include <boost/assert.hpp>

namespace piejam::audio::engine
{

namespace
{

class identity_processor final : public named_processor
{
public:
    identity_processor(std::string_view const& name)
        : named_processor(name)
    {
    }

    auto type_name() const -> std::string_view override
    {
        return "audio_identity";
    }

    auto num_inputs() const -> std::size_t override { return 1; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(process_context const&) override
    {
        BOOST_ASSERT_MSG(
                false,
                "Should be removed from graph before executing.");
    }
};

} // namespace

auto
make_identity_processor(std::string_view const& name)
        -> std::unique_ptr<processor>
{
    return std::make_unique<identity_processor>(name);
}

bool
is_identity_processor(processor const& p)
{
    return typeid(p) == typeid(identity_processor);
}

} // namespace piejam::audio::engine
