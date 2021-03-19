// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/semaphore.h>

#include <semaphore.h>

#include <boost/assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include <system_error>

namespace piejam::thread
{

struct semaphore::impl
{
    sem_t sem;
};

semaphore::semaphore()
    : semaphore(0)
{
}

semaphore::semaphore(unsigned init_value)
    : m_impl(std::make_unique<impl>())
{
    auto pimpl = std::make_unique<impl>();
    if (-1 == ::sem_init(&pimpl->sem, 0, init_value))
        throw std::system_error(errno, std::generic_category());

    m_impl = std::move(pimpl);
}

semaphore::~semaphore()
{
    if (m_impl.get())
        ::sem_destroy(&m_impl->sem);
}

void
semaphore::acquire() noexcept
{
    BOOST_ASSERT(m_impl);
    ::sem_wait(&m_impl->sem);
}

bool
semaphore::try_acquire() noexcept
{
    BOOST_ASSERT(m_impl);
    return 0 == ::sem_trywait(&m_impl->sem);
}

void
semaphore::release() noexcept
{
    BOOST_ASSERT(m_impl);
    ::sem_post(&m_impl->sem);
}

} // namespace piejam::thread
