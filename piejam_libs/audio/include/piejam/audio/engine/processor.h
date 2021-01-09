// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <functional>
#include <span>
#include <string_view>

namespace piejam::audio::engine
{

class processor
{
public:
    using event_ports = std::span<event_port const>;

    virtual ~processor() = default;

    virtual auto type_name() const -> std::string_view = 0;
    virtual auto name() const -> std::string_view = 0;

    virtual auto num_inputs() const -> std::size_t = 0;
    virtual auto num_outputs() const -> std::size_t = 0;

    virtual auto event_inputs() const -> event_ports = 0;
    virtual auto event_outputs() const -> event_ports = 0;

    virtual void process(process_context const&) = 0;
};

} // namespace piejam::audio::engine
