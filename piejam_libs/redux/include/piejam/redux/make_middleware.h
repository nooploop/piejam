// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

namespace piejam::redux
{

template <class MW, class... Args>
auto
make_middleware(Args&&... args)
{
    auto m = std::make_shared<MW>(std::forward<Args>(args)...);
    return [m = std::move(m)](auto const& action) { (*m)(action); };
}

} // namespace piejam::redux
