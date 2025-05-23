// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <string_view>

namespace piejam::algorithm
{

[[nodiscard]]
inline auto
escape_html(std::string_view const src) -> std::string
{
    constexpr auto grow_factor{1.05f};

    std::string dst;
    dst.reserve(static_cast<std::size_t>(
            grow_factor * static_cast<float>(src.size())));

    for (char const ch : src)
    {
        switch (ch)
        {
            case '&':
                dst += "&amp;";
                break;

            case '\'':
                dst += "&apos;";
                break;

            case '"':
                dst += "&quot;";
                break;

            case '<':
                dst += "&lt;";
                break;

            case '>':
                dst += "&gt;";
                break;

            default:
                dst += ch;
                break;
        }
    }

    return dst;
}

} // namespace piejam::algorithm
