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

#include <piejam/system/device.h>

#include <boost/assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <system_error>
#include <utility>

namespace piejam::system
{

device::device(std::filesystem::path const& pathname)
    : m_fd(::open(pathname.c_str(), O_RDONLY))
{
    if (m_fd < 0)
        throw std::system_error(errno, std::generic_category());
}

device::device(device&& other) noexcept
    : m_fd(std::exchange(other.m_fd, invalid))
{
}

device::~device()
{
    if (m_fd != invalid)
        ::close(m_fd);
}

auto
device::operator=(device&& other) -> device&
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
device::ioctl(unsigned long const request) const
{
    BOOST_ASSERT(m_fd != invalid);
    if (-1 == ::ioctl(m_fd, request))
        throw std::system_error(errno, std::generic_category());
}

void
device::ioctl(
        unsigned long const request,
        void* const p,
        std::size_t const size) const
{
    BOOST_ASSERT(m_fd != invalid);
    BOOST_ASSERT(_IOC_SIZE(request) == size);
    boost::ignore_unused(size);

    if (-1 == ::ioctl(m_fd, request, p))
        throw std::system_error(errno, std::generic_category());
}

template <>
void
device::ioctl(unsigned long request, device const& other) const
{
    ioctl(request, reinterpret_cast<void*>(other.m_fd), sizeof(other.m_fd));
}

} // namespace piejam::system
