// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::runtime::fx
{

enum class internal : unsigned
{
    gain,
    scope,
    spectrum
};

} // namespace piejam::runtime::fx
