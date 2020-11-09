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

#include <filesystem>

namespace piejam::system
{

class ioctl_device
{
public:
    static constexpr int invalid = -1;

    ioctl_device() noexcept = default;
    ioctl_device(std::filesystem::path const& pathname);
    ioctl_device(ioctl_device const&) = delete;
    ioctl_device(ioctl_device&& other) noexcept;

    ~ioctl_device();

    auto operator=(ioctl_device const&) -> ioctl_device& = delete;
    auto operator=(ioctl_device&& other) -> ioctl_device&;

    explicit operator bool() const noexcept { return m_fd != invalid; }

    void ioctl(unsigned long request) const;

    template <class T>
    auto ioctl(unsigned long request) const -> T
    {
        T x{};
        ioctl(request, &x, sizeof(T));
        return x;
    }

    template <class T>
    void ioctl(unsigned long request, T& x) const
    {
        ioctl(request, &x, sizeof(T));
    }

private:
    void ioctl(unsigned long request, void* p, std::size_t size) const;

    int m_fd{invalid};
};

template <>
void ioctl_device::ioctl(unsigned long request, ioctl_device&) const = delete;

template <>
void
ioctl_device::ioctl(unsigned long request, ioctl_device const& other) const;

} // namespace piejam::system
