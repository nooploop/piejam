// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
    {
        throw std::system_error(errno, std::generic_category());
    }
}

device::device(device&& other) noexcept
    : m_fd(std::exchange(other.m_fd, invalid))
{
}

device::~device()
{
    if (m_fd != invalid)
    {
        ::close(m_fd);
    }
}

auto
device::operator=(device&& other) noexcept -> device&
{
    if (m_fd != invalid)
    {
        BOOST_VERIFY(!::close(m_fd));
    }

    m_fd = std::exchange(other.m_fd, invalid);
    return *this;
}

auto
device::ioctl(unsigned long const request) noexcept -> std::error_code
{
    BOOST_ASSERT(m_fd != invalid);
    if (-1 == ::ioctl(m_fd, request))
    {
        return std::error_code(errno, std::generic_category());
    }

    return {};
}

auto
device::ioctl(
        unsigned long const request,
        void* const p,
        std::size_t const size) noexcept -> std::error_code
{
    BOOST_ASSERT(m_fd != invalid);
    BOOST_ASSERT(_IOC_SIZE(request) == size);
    boost::ignore_unused(size);

    if (-1 == ::ioctl(m_fd, request, p))
    {
        return std::error_code(errno, std::generic_category());
    }

    return {};
}

template <>
auto
device::ioctl(unsigned long request, device const& other) noexcept
        -> std::error_code
{
    return ioctl(
            request,
            reinterpret_cast<void*>(other.m_fd),
            sizeof(other.m_fd));
}

auto
device::read(std::span<std::byte> const buffer) noexcept
        -> outcome::std_result<std::size_t>
{
    auto const res = ::read(m_fd, buffer.data(), buffer.size());
    if (res < 0)
    {
        return std::error_code(errno, std::generic_category());
    }

    return static_cast<std::size_t>(res);
}

auto
device::set_nonblock(bool const set) -> std::error_code
{
    int flags = ::fcntl(m_fd, F_GETFL);
    if (-1 == flags)
    {
        return std::error_code(errno, std::generic_category());
    }

    if (set)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags &= ~O_NONBLOCK;
    }

    if (-1 == ::fcntl(m_fd, F_SETFL, flags))
    {
        return std::error_code(errno, std::generic_category());
    }

    return {};
}

} // namespace piejam::system
