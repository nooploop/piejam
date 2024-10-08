// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

namespace piejam
{

template <class T>
using pimpl = std::unique_ptr<T, void (*)(T*)>;

template <class T, class... Args>
auto
make_pimpl(Args&&... args) -> pimpl<T>
{
    return pimpl<T>{new T{std::forward<Args>(args)...}, [](T* p) { delete p; }};
}

} // namespace piejam
