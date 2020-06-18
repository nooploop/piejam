// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <type_traits>
#include <utility>

namespace piejam::audio
{

// Utility class like std::pair. Just to make it easier to define and use
// a pair in audio context.
template <class T>
struct pair : private std::pair<T, T>
{
    using type = T;

    pair(T const& t)
        : std::pair<T, T>(t, t)
    {
    }

    using std::pair<T, T>::pair;
    using std::pair<T, T>::operator=;
    using std::pair<T, T>::swap;

    constexpr auto left() const noexcept -> T const& { return this->first; }
    constexpr auto left() noexcept -> T& { return this->first; }

    constexpr auto right() const noexcept -> T const& { return this->second; }
    constexpr auto right() noexcept -> T& { return this->second; }
};

} // namespace piejam::audio
