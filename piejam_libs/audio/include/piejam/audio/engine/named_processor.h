// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/processor.h>

#include <string>

namespace piejam::audio::engine
{

class named_processor : public processor
{
public:
    named_processor(std::string_view const name = {})
        : m_name(name)
    {
    }

    [[nodiscard]] auto name() const noexcept -> std::string_view override
    {
        return m_name;
    }

private:
    std::string const m_name;
};

} // namespace piejam::audio::engine
