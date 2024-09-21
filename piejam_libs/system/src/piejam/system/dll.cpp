// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/system/dll.h>

#include <boost/assert.hpp>

#include <dlfcn.h>

#include <system_error>
#include <utility>

namespace piejam::system
{

dll::dll(std::filesystem::path const& file)
    : m_handle(dlopen(file.c_str(), RTLD_LAZY))
{
    if (!m_handle)
    {
        throw std::system_error(errno, std::system_category(), dlerror());
    }
}

dll::dll(dll&& other) noexcept
    : m_handle(std::exchange(other.m_handle, nullptr))
{
}

dll::~dll()
{
    if (m_handle)
    {
        BOOST_VERIFY(!dlclose(m_handle));
    }
}

auto
dll::operator=(dll&& other) -> dll&
{
    if (m_handle)
    {
        if (dlclose(m_handle))
        {
            throw std::system_error(errno, std::system_category(), dlerror());
        }
    }

    m_handle = std::exchange(other.m_handle, nullptr);
    return *this;
}

auto
dll::symbol(std::string_view const name) const -> void*
{
    BOOST_ASSERT(m_handle);
    void* const res = dlsym(m_handle, name.data());
    if (!res)
    {
        throw std::system_error(errno, std::system_category(), dlerror());
    }
    return res;
}

} // namespace piejam::system
