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

#include <piejam/system/dll.h>

#include <boost/assert.hpp>

#include <dlfcn.h>

#include <system_error>

namespace piejam::system
{

dll::dll(std::filesystem::path const& file)
    : m_handle(dlopen(file.c_str(), RTLD_LAZY))
{
    if (!m_handle)
        throw std::system_error(errno, std::system_category(), dlerror());
}

dll::dll(dll&& other) noexcept
    : m_handle(std::exchange(other.m_handle, nullptr))
{
}

dll::~dll()
{
    if (m_handle)
        BOOST_VERIFY(!dlclose(m_handle));
}

auto
dll::operator=(dll&& other) -> dll&
{
    if (m_handle)
    {
        if (dlclose(m_handle))
            throw std::system_error(errno, std::system_category(), dlerror());
    }

    m_handle = std::exchange(other.m_handle, nullptr);
    return *this;
}

auto
dll::symbol(std::string_view const& name) const -> void*
{
    BOOST_ASSERT(m_handle);
    void* const res = dlsym(m_handle, name.data());
    if (!res)
        throw std::system_error(errno, std::system_category(), dlerror());
    return res;
}

} // namespace piejam::system
