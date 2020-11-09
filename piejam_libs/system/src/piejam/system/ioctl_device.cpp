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

#include <piejam/system/ioctl_device.h>

#include <boost/core/ignore_unused.hpp>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cassert>
#include <system_error>

namespace piejam::system
{

ioctl_device::ioctl_device(std::filesystem::path const& pathname)
    : m_fd(::open(pathname.c_str(), O_RDONLY))
{
    if (m_fd < 0)
        throw std::system_error(errno, std::generic_category());
}

ioctl_device::ioctl_device(ioctl_device&& other) noexcept
    : m_fd(other.m_fd)
{
    other.m_fd = invalid;
}

ioctl_device::~ioctl_device()
{
    if (m_fd != invalid)
        ::close(m_fd);
}

auto
ioctl_device::operator=(ioctl_device&& other) -> ioctl_device&
{
    if (m_fd != invalid)
    {
        if (-1 == ::close(m_fd))
            throw std::system_error(errno, std::generic_category());
    }

    m_fd = std::exchange(other.m_fd, invalid);
    return *this;
}

void
ioctl_device::ioctl(unsigned long const request) const
{
    assert(m_fd != invalid);
    if (-1 == ::ioctl(m_fd, request))
        throw std::system_error(errno, std::generic_category());
}

void
ioctl_device::ioctl(
        unsigned long const request,
        void* const p,
        std::size_t const size) const
{
    assert(m_fd != invalid);
    assert(_IOC_SIZE(request) == size);
    boost::ignore_unused(size);

    if (-1 == ::ioctl(m_fd, request, p))
        throw std::system_error(errno, std::generic_category());
}

template <>
void
ioctl_device::ioctl(unsigned long request, ioctl_device const& other) const
{
    ioctl(request, reinterpret_cast<void*>(other.m_fd), sizeof(other.m_fd));
}

} // namespace piejam::system
