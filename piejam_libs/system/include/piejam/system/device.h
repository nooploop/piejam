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
#include <system_error>

namespace piejam::system
{

class device
{
public:
    device() noexcept = default;
    device(std::filesystem::path const& pathname);
    device(device const&) = delete;
    device(device&& other) noexcept;

    ~device();

    auto operator=(device const&) -> device& = delete;
    auto operator=(device&& other) noexcept -> device&;

    explicit operator bool() const noexcept { return m_fd != invalid; }

    void ioctl(unsigned long request) const;

    template <class T>
    void ioctl(unsigned long request, T& x) const
    {
        if (auto err = ioctl(request, &x, sizeof(T)))
            throw std::system_error(err);
    }

private:
    [[nodiscard]] auto
    ioctl(unsigned long request, void* p, std::size_t size) const noexcept
            -> std::error_code;

    static constexpr int invalid = -1;

    int m_fd{invalid};
};

template <>
void device::ioctl(unsigned long request, device&) const = delete;

template <>
void device::ioctl(unsigned long request, device const& other) const;

} // namespace piejam::system
